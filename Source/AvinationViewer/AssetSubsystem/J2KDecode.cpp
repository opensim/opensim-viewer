// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "J2KDecode.h"
#include "ImageUtils.h"
#include "../Utils/MemStream.h"

void showMsg(const char *msg, void *userData)
{
    UE_LOG(LogTemp, Error, TEXT("%s"), *FString(msg));
}

J2KDecode::J2KDecode()
{
    image = 0;
}

J2KDecode::~J2KDecode()
{
    if (image)
        opj_image_destroy(image);
}

 bool J2KDecode::Decode(TArray<uint8_t> data)
{
    if (data.Num() == 0)
        return 0;
    
    MemStream *memstr = new MemStream(data.GetData(), data.Num());
    
    opj_dparameters_t parameters;
    opj_codec_t *codec;
    image = 0;
    opj_stream_t *str;
    
    opj_set_default_decoder_parameters(&parameters);
    
    codec = opj_create_decompress(OPJ_CODEC_J2K);
    if (!codec)
    {
        delete memstr;
        return false;
    }
    
    OPJ_BOOL success = opj_setup_decoder(codec, &parameters);
    if (!success)
    {
        opj_destroy_codec(codec);
        delete memstr;
        return false;
    }
    
    str = opj_stream_default_create(true);
    if (!str)
    {
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
        delete memstr;
        return false;
    }
    
    opj_stream_set_read_function(str, MemStream::read);
    opj_stream_set_seek_function(str, MemStream::seek);
    opj_stream_set_skip_function(str, MemStream::skip);
    opj_stream_set_user_data(str, memstr, MemStream::free);
    opj_stream_set_user_data_length(str, data.Num());
    //opj_set_info_handler(codec, showMsg, 0);
    //opj_set_warning_handler(codec, showMsg, 0);
    //opj_set_error_handler(codec, showMsg, 0);
    
    success = opj_read_header(str, codec, &image);
    if (!success)
    {
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
        delete memstr;
        return false;
    }
    
    success = opj_decode(codec, str, image);
    if (!success)
    {
        if (image)
            opj_image_destroy(image);
        image = 0;
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
        delete memstr;
        return false;
    }
    
    //    printf("NumComps %d ColorSpace %d\n", image->numcomps, image->color_space);
    
    //printf("%d %d %d %d\n", image->comps[0].w, image->comps[0].h, image->comps[0].bpp, image->comps[0].factor);
    
    opj_destroy_codec(codec);
    opj_stream_destroy(str);
    
    if (!image)
        return false;
    
    return true;
}

UTexture2D *J2KDecode::CreateTexture(UObject *outer, FGuid id)
{
    const int32_t *a = 0;
    if (image->numcomps > 3)
        a = image->comps[3].data;
    
    const int32_t *r, *g = 0, *b = 0;
    
    r = image->comps[0].data;
    
    if (image->numcomps > 1)
    {
        g = image->comps[1].data;
        b = image->comps[2].data;
    }
    
    UTexture2D *tex = CreateTexture2D(image->comps[0].w,
                                      image->comps[0].h,
                                      r,
                                      g,
                                      b,
                                      a,
                                      outer,
                                      id.ToString(),
                                      RF_NoFlags);
    return tex;
}

UTexture2D* J2KDecode::CreateTexture2D(int32_t SrcWidth, int32_t SrcHeight, const int32_t *r, const int32_t *g, const int32_t *b, const int32_t *a, UObject* Outer, const FString& Name, const EObjectFlags &Flags)
{
    UTexture2D* Tex2D;
    
    Tex2D = NewObject<UTexture2D>(Outer, FName(*Name), Flags);
    Tex2D->Source.Init(SrcWidth, SrcHeight, /*NumSlices=*/ 1, /*NumMips=*/ 1, TSF_BGRA8);
    
    bool hasAlpha = false;
    
    // Create base mip for the texture we created.
    uint8* MipData = Tex2D->Source.LockMip(0);
    for (int32_t y=0 ; y < SrcHeight ; y++)
    {
        //uint8* DestPtr = &MipData[(SrcHeight - 1 - y) * SrcWidth * sizeof(FColor)];
        uint8* DestPtr = &MipData[y * SrcWidth * sizeof(FColor)];
        
        for(int32_t x=0; x<SrcWidth; x++)
        {
            if (g)
            {
                *DestPtr++ = (uint8_t)(*(b++));
                *DestPtr++ = (uint8_t)(*(g++));
                *DestPtr++ = (uint8_t)(*(r++));
            }
            else
            {
                *DestPtr++ = (uint8_t)(*r);
                *DestPtr++ = (uint8_t)(*r);
                *DestPtr++ = (uint8_t)(*(r++));
            }
            
            if (a != 0)
            {
                uint8_t alpha = (uint8_t)(*(a++));
                if (alpha < 0xfc)
                    hasAlpha = true;
                else
                    alpha = 0xff;
                *DestPtr++ = alpha;
            }
            else
            {
                *DestPtr++ = 0xFF;
            }
        }
    }
    
    Tex2D->Source.UnlockMip(0);
    
    // Set compression options.
    Tex2D->SRGB = true;
    Tex2D->CompressionSettings	= TC_Default;
    if (a == 0 || !hasAlpha)
        Tex2D->CompressionNoAlpha = true;
    
    Tex2D->DeferCompression	= true;
    
    return Tex2D;

//    return nullptr;
}
