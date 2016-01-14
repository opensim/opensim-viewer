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

bool J2KDecode::Decode(TSharedPtr<TArray<uint8_t>, ESPMode::ThreadSafe> data)
{
    if (data->Num() == 0)
        return 0;
    
    MemStream memstr(data->GetData(), data->Num());
    
    opj_dparameters_t parameters;
    opj_codec_t *codec;
    image = 0;
    opj_stream_t *str;
    
    opj_set_default_decoder_parameters(&parameters);
    
    codec = opj_create_decompress(OPJ_CODEC_J2K);
    if (!codec)
    {
        return false;
    }
    
    OPJ_BOOL success = opj_setup_decoder(codec, &parameters);
    if (!success)
    {
        opj_destroy_codec(codec);
        return false;
    }
    
    str = opj_stream_default_create(true);
    if (!str)
    {
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
        return false;
    }
    
    opj_stream_set_read_function(str, MemStream::read);
    opj_stream_set_seek_function(str, MemStream::seek);
    opj_stream_set_skip_function(str, MemStream::skip);
    opj_stream_set_user_data(str, &memstr, 0);
    opj_stream_set_user_data_length(str, data->Num());
    //opj_set_info_handler(codec, showMsg, 0);
    //opj_set_warning_handler(codec, showMsg, 0);
    //opj_set_error_handler(codec, showMsg, 0);
    
    success = opj_read_header(str, codec, &image);
    if (!success)
    {
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
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
