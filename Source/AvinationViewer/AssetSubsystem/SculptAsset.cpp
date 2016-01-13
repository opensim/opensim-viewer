// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "SculptAsset.h"
#include "../Meshing/SculptMesher.h"

SculptAsset::SculptAsset()
{
    decode.BindRaw(this, &SculptAsset::DecodeImage);
    mainProcess.BindRaw(this, &SculptAsset::Process);
}

SculptAsset::~SculptAsset()
{
    if (image)
        opj_image_destroy(image);
    image = 0;
}

void SculptAsset::DecodeImage()
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
    
    if (image->numcomps < 3)
    {
        opj_image_destroy(image);
        image = 0;
        throw std::exception();
    }
}

void SculptAsset::Process()
{
    if (!image)
        throw std::exception();

    if (image->numcomps < 3)
        throw std::exception();

    int w = image->comps[0].w;
    int h = image->comps[0].h;

    float pixScale = 1.0f / 255.0f;

    sculptRows.Empty();
    OPJ_INT32 *r = image->comps[0].data;
    OPJ_INT32 *g = image->comps[1].data;
    OPJ_INT32 *b = image->comps[2].data;
    for (int i = 0; i < h; i++)
        sculptRows.AddDefaulted();

    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            FVector c = FVector((float)(*r++ & 0xff) * pixScale - 0.5f,
                (float)(*g++ & 0xff) * pixScale - 0.5f,
                (float)(*b++ & 0xff) * pixScale - 0.5f);
            sculptRows[h - 1 - i].Add(c);
        }
    }

    opj_image_destroy(image);
    image = 0;
}

