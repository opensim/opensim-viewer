// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "SculptAsset.h"
#include "../Meshing/SculptMesher.h"

SculptAsset::SculptAsset()
{
    mainProcess.BindRaw(this, &SculptAsset::DecodeImage);
}

SculptAsset::~SculptAsset()
{
    if (image)
        opj_image_destroy(image);
    image = 0;
}

void SculptAsset::DecodeImage()
{
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
    
    if (image->numcomps < 3)
    {
        opj_image_destroy(image);
        image = 0;
        throw std::exception();
    }
}

