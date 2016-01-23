// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "AssetBase.h"
#include "J2KDecode.h"

/**
 * 
 */
class AVINATIONVIEWER_API TextureAsset : public AssetBase
{
public:
	TextureAsset();
	~TextureAsset();
    
    UTexture2D *tex = 0;
    bool hasAlpha = false;

    bool GetFromCache(const TCHAR *file) override;
private:

    UTexture2D *baseTexture;

    void DecodeImage();
    void PreProcess();
    void Process();
    void PostProcess();

    void LoadFromFile(FString file);
    
    UTexture2D* CreateTexture();
    
    int w, h;
    opj_image *image = 0;
    TArray<uint8_t*> texBuffer;
    int32_t nlevels = 0;
};
