// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "TextureAsset.h"

TextureAsset::TextureAsset()
{
    decode.BindRaw(this, &TextureAsset::DecodeImage);
    preProcess.BindRaw(this, &TextureAsset::PreProcess);
    mainProcess.BindRaw(this, &TextureAsset::Process);
    postProcess.BindRaw(this, &TextureAsset::PostProcess);
}

TextureAsset::~TextureAsset()
{
    if (image)
        opj_image_destroy(image);
    image = 0;
}

void TextureAsset::DecodeImage()
{
    AssetBase::Decode();
    
    J2KDecode idec;
    if (!idec.Decode(stageData))
    {
        throw std::exception();
    }
    
    stageData.Reset();
    
    image = idec.image;
    idec.image = 0;
    
    if (!image)
        throw std::exception();
    
    if (image->numcomps < 1)
        throw std::exception();

    w = image->comps[0].w;
    h = image->comps[0].h;
    if (w < 0 || h < 0)
    {
        opj_image_destroy(image);
        image = 0;
        throw std::exception();
    }


}

void TextureAsset::PreProcess()
{
    tex = UTexture2D::CreateTransient(w, h);
//    tex->MipGenSettings = TextureMipGenSettings::TMGS_Blur4;
    tex->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

    // this needs to be set acording to use
 //   tex->SRGB = 0; // normal map 
    tex->SRGB = true; // difuse maps

    tex->UpdateResource();
    tex->AddToRoot();
    upd = new FUpdateTextureRegion2D(0, 0, 0, 0, w, h);
}

void TextureAsset::Process()
{
    texBuffer = new uint8_t[w * h * 4];

    int32_t *r, *g, *b, *a;
    r = 0;
    g = 0;
    b = 0;
    a = 0;

    switch (image->numcomps)
    {
    case 1: // Straight greyscale
        r = image->comps[0].data;
        break;
    case 2: // Greyscale w/alpha
        r = image->comps[0].data;
        a = image->comps[1].data;
        break;
    case 3: // RGB
        r = image->comps[0].data;
        g = image->comps[1].data;
        b = image->comps[2].data;
        break;
    case 4: // RGBA
        r = image->comps[0].data;
        g = image->comps[1].data;
        b = image->comps[2].data;
        a = image->comps[3].data;
        break;
    }

    hasAlpha = false;
    int pixels = w * h;

    uint8_t *dest = texBuffer;
    for (int i = 0; i < pixels; i++)
    {
        if (!g)
        {
            *dest++ = (uint8_t)*r;
            *dest++ = (uint8_t)*r;
        }
        else
        {
            *dest++ = (uint8_t)*(b++);
            *dest++ = (uint8_t)*(g++);
        }
        *dest++ = (uint8_t)*(r++);

        if (a)
        {
            if (*a < 0xf8)
                hasAlpha = true;
            *dest++ = (uint8_t)*(a++);
        }
        else
        {
            *dest++ = 0xff;
        }
    }

    opj_image_destroy(image);
    image = 0;
}

void TextureAsset::PostProcess()
{
    if (state == AssetBase::Failed)
        return;
    
    struct FUpdateTextureRegionsData
    {
        FTexture2DResource* Texture2DResource;
        int32 MipIndex;
        uint32 NumRegions;
        FUpdateTextureRegion2D* Regions;
        uint32 SrcPitch;
        uint32 SrcBpp;
        uint8* SrcData;
    };
    
    FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;
    
    RegionData->Texture2DResource = (FTexture2DResource*)tex->Resource;
    RegionData->MipIndex = 0;
    RegionData->NumRegions = 1;
    RegionData->Regions = upd;
    RegionData->SrcPitch = w * 4;
    RegionData->SrcBpp = 4;
    RegionData->SrcData = texBuffer;
    
    ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
       UpdateTextureRegionsData,
       FUpdateTextureRegionsData*, RegionData, RegionData,
       {
           for (uint32 RegionIndex = 0; RegionIndex < RegionData->NumRegions; ++RegionIndex)
           {
               int32 CurrentFirstMip = RegionData->Texture2DResource->GetCurrentFirstMip();
               if (RegionData->MipIndex >= CurrentFirstMip)
               {
                   RHIUpdateTexture2D(
                                      RegionData->Texture2DResource->GetTexture2DRHI(),
                                      RegionData->MipIndex - CurrentFirstMip,
                                      RegionData->Regions[RegionIndex],
                                      RegionData->SrcPitch,
                                      RegionData->SrcData
                                      + RegionData->Regions[RegionIndex].SrcY * RegionData->SrcPitch
                                      + RegionData->Regions[RegionIndex].SrcX * RegionData->SrcBpp
                                      );
               }
           }
           
           FMemory::Free(RegionData->Regions);
           FMemory::Free(RegionData->SrcData);
           delete RegionData;
       }
    );
}