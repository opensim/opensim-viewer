// (c) 2016 Avination Virtual Limited. All rights reserved.
#include "AvinationViewer.h"
#include "TextureAsset.h"
#include "AvinationViewerGameMode.h"


struct MBB :FByteBulkData
{
    void SetFilename(FString name);
#if WITH_EDITOR
    void setAr(FArchive *ar);
#endif
};

void MBB::SetFilename(FString name)
{
    Filename = name;
}

#if WITH_EDITOR
void MBB::setAr(FArchive *ar)
{
    AttachedAr = ar;
}
#endif

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

//    worktex->Serialize();
}

void TextureAsset::Process()
{
    texBuffer.Empty();
    texBuffer.AddZeroed(13);

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
    if (mindimension > w)
        mindimension = w;
    int sw = w;
    int sh = h;

    unsigned int *tmpline = new unsigned int[w * 4];

    while (mindimension > 1 && nlevels < 13)
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

    tex = TextureAsset::CreateTexture();
    if (tex == nullptr)
        state = AssetBase::Failed;
}

void TextureAsset::PostProcess()
{
    if (state == AssetBase::Failed)
        return;

    tex->UpdateResource();

    tex->AddToRoot();

}
UTexture2D* TextureAsset::CreateTexture()
{
    bool normalMap = false;
    FString sid = id.ToString();
    FString ppath = FPaths::GameDir();

    ppath = FPaths::Combine(*ppath, TEXT("cache/"),*sid);
    FPaths::MakeStandardFilename(ppath);

//    UE_LOG(LogTemp, Warning, TEXT("%s"), *ppath);

    FArchive* ar = IFileManager::Get().CreateFileWriter(*ppath);
    ar->ArIsSaving = true;
    ar->ArIsPersistent = true;

    UTexture2D* Texture = nullptr;
    Texture = NewObject<UTexture2D>(GetTransientPackage(),*sid, RF_Standalone | RF_Public);

    if (!Texture)
    {
        return nullptr;
    }

    Texture->PlatformData = new FTexturePlatformData();
    Texture->PlatformData->SizeX = w;
    Texture->PlatformData->SizeY = h;
    Texture->PlatformData->PixelFormat = EPixelFormat::PF_B8G8R8A8;
    Texture->PlatformData->NumSlices = 1;
    Texture->bForceMiplevelsToBeResident = false;
    Texture->bIsStreamable = true;

    int tmp = 0;
    for (int level = 0; level < nlevels; level++)
    {
        int lw = w >> level;
        int lh = h >> level;

        FTexture2DMipMap *Mip = new FTexture2DMipMap();
        void* NewMipData = Mip->BulkData.Lock(LOCK_READ_WRITE);
        Mip->SizeX = lw;
        Mip->SizeY = lh;
        int tsize = lw * lh * 4;
        NewMipData = Mip->BulkData.Realloc(tsize);
        FMemory::Memcpy(NewMipData, (void*)texBuffer[level], tsize);
        Texture->PlatformData->Mips.Add(Mip);
        Mip->BulkData.Unlock();
        MBB* b = (MBB*)&Mip->BulkData;
        b->SetFilename(ppath);
        b->SetBulkDataFlags(EBulkDataFlags::BULKDATA_SerializeCompressedZLIB);
        Mip->BulkData.Serialize(*ar,0);
        tmp = 0;
    }

    ar->FlushCache();
    ar->Close();
    delete ar;

    if (nlevels > 6)
    {
        int min = nlevels - 6;
        for (int level = 0; level < min; level++)
        {
            FTexture2DMipMap *Mip = &Texture->PlatformData->Mips[level];
            MBB* b = (MBB*)&Mip->BulkData;
            b->SetBulkDataFlags(EBulkDataFlags::BULKDATA_SingleUse);
            Mip->BulkData.Lock(LOCK_READ_WRITE);
            Mip->BulkData.Unlock();
            b->SetBulkDataFlags(EBulkDataFlags::BULKDATA_SerializeCompressedZLIB);
            tmp++;
        }
    }

// another hack ... a absurd

#if WITH_EDITOR
    FArchive* readAr = IFileManager::Get().CreateFileReader(*ppath, FILEREAD_Silent);
    for (int level = 0; level < nlevels; level++)
    {
        MBB* b = (MBB*)&Texture->PlatformData->Mips[level].BulkData;
        readAr->AttachBulkData(tex,&Texture->PlatformData->Mips[level].BulkData);
        b->setAr(readAr);
    }
#endif

    if (normalMap)
    {
        Texture->LODGroup = TEXTUREGROUP_WorldNormalMap;
        Texture->CompressionSettings = TC_Normalmap;
        Texture->SRGB = false;
    }
    else
    {
        Texture->LODGroup = TEXTUREGROUP_World;
        Texture->CompressionSettings = TC_Default;
        Texture->SRGB = true;
    }

    return Texture;
}


