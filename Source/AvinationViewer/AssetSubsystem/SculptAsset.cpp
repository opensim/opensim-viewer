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
        delete image;
}

void SculptAsset::DecodeImage()
{
    J2KDecode *idec = new J2KDecode();
    if (!idec->Decode(stageData))
    {
        throw std::exception();
    }
    
    stageData.Reset();
    
    image = idec->image;
    
    if (!image)
        throw std::exception();
    
    if (image->numcomps < 3)
        throw std::exception();
}

