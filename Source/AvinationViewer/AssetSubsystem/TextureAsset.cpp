// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "TextureAsset.h"

TextureAsset::TextureAsset()
{
    decode.BindRaw(this, &TextureAsset::DecodeImage);
//    preProcess.BindRaw(this, &TextureAsset::PreProcess);
    mainProcess.BindRaw(this, &TextureAsset::Process);
    postProcess.BindRaw(this, &TextureAsset::PostProcess);
}

TextureAsset::~TextureAsset()
{
    if (image)
        opj_image_destroy(image);
    image = 0;
    if (nlevels > 0)
    {
        for (int i = 0; i < nlevels; i++)
        {
            if (texBuffer[i])
                FMemory::Free(texBuffer[i]);
            texBuffer[i] = nullptr;
        }
    }
    texBuffer.Empty();
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
}

void TextureAsset::Process()
{
    texBuffer.Empty();
    texBuffer.AddZeroed(12);

    texBuffer[0] = new uint8_t[w * h * 4];

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

    uint8_t *dest = texBuffer[0];
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

    nlevels = 1;

    int mindimension = h;
    if (mindimension < w)
        mindimension = w;
    int sw = w;
    int sh = h;

    unsigned int *tmpline = new unsigned int[w * 4];

    while (mindimension > 32 && nlevels < 10)
    {
        texBuffer[nlevels] = new uint8_t[sw * sh]; // reducion will be by 2x2 so number of components included

        uint8_t *dest = texBuffer[nlevels];
        uint8_t *src = texBuffer[nlevels - 1];

        unsigned int *destline;

        sh >>= 1; 
        sw >>= 1;
        mindimension >>= 1;

        for (int j = 0; j < sh; j++)
        {
            destline = tmpline;
            for (int i = 0; i < sw; i++)
            {
                *destline++ = *src++;
                *destline++ = *src++;
                *destline++ = *src++;
                *destline   = *src++;

                destline -= 3;
                *destline++ += *src++;
                *destline++ += *src++;
                *destline++ += *src++;
                *destline++ += *src++;
            }

            destline = tmpline;
            for (int i = 0; i < sw; i++)
            {
                *destline++ += *src++;
                *destline++ += *src++;
                *destline++ += *src++;
                *destline   += *src++;

                destline -= 3;
                *destline++ += *src++;
                *destline++ += *src++;
                *destline++ += *src++;
                *destline   += *src++;

                destline -= 3;
                *dest++ = (uint8_t)(*destline++ >> 2);
                *dest++ = (uint8_t)(*destline++ >> 2);
                *dest++ = (uint8_t)(*destline++ >> 2);
                *dest++ = (uint8_t)(*destline++ >> 2);
            }
        }

        nlevels++;
    }
    FMemory::Free(tmpline);
}

void TextureAsset::PostProcess()
{
    if (state == AssetBase::Failed)
        return;

    tex = UTexture2D::CreateTransient(w, h);
    tex->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;

    // this needs to be set acording to use
    //   tex->SRGB = 0; // normal map 
    tex->SRGB = true; // difuse maps

    // for some reason this original doesn't work
    FTexture2DMipMap Mip = tex->PlatformData->Mips[0];
    Mip.BulkData.RemoveBulkData();
    tex->PlatformData->Mips.Empty();

    // force just one test
    //nlevels = 1;

    for (int level = 0; level < nlevels; level++)
    {
        int lw = w >> level;
        int lh = h >> level;

        FTexture2DMipMap *Mip = new FTexture2DMipMap();
        Mip->BulkData.Lock(LOCK_READ_WRITE);
        Mip->SizeX = lw;
        Mip->SizeY = lh;
        int tsize = lw * lh * 4;
        void* NewMipData = Mip->BulkData.Realloc(tsize);
        FMemory::Memcpy(NewMipData, (void*)texBuffer[level], tsize);
        tex->PlatformData->Mips.Add(Mip);
        Mip->BulkData.Unlock();
    }

    tex->UpdateResource();
    tex->AddToRoot();
/*
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

    for (int level = 0; level < nlevels; level++)
    {
        int lw = w >> level;
        int lh = h >> level;

        FUpdateTextureRegion2D* upd = new FUpdateTextureRegion2D(0, 0, 0, 0, lw, lh);

        FUpdateTextureRegionsData* RegionData = new FUpdateTextureRegionsData;

        RegionData->Texture2DResource = (FTexture2DResource*)tex->Resource;
        RegionData->MipIndex = level;
        RegionData->NumRegions = 1;
        RegionData->Regions = upd;
        RegionData->SrcPitch = lw * 4;
        RegionData->SrcBpp = 4;
        RegionData->SrcData = texBuffer[level];

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
*/
}