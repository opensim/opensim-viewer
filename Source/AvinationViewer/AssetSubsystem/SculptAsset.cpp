/*
 * Copyright (c) Contributors, http://opensimulator.org/
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the OpenSimulator Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
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

