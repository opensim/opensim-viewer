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
    
    UPROPERTY()
    UTexture2D *tex = 0;
    bool hasAlpha = false;
    
private:
    void DecodeImage();
    void PreProcess();
    void Process();
    void PostProcess();
    
    int w, h;
    opj_image *image = 0;
    uint8_t *texBuffer = 0;
    FUpdateTextureRegion2D *upd = 0;
};
