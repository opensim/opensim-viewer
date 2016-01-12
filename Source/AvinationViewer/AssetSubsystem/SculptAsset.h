// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "MeshableAsset.h"
#include "J2KDecode.h"

/**
 * 
 */
class AVINATIONVIEWER_API SculptAsset : public MeshableAsset
{
public:
	SculptAsset();
	virtual ~SculptAsset();
    
    opj_image *image = 0;
    
private:
    void DecodeImage();
};
