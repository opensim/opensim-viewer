// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "AssetBase.h"
#include "SubmeshData.h"

/**
 * 
 */
class AVINATIONVIEWER_API MeshableAsset : public AssetBase
{
public:
	MeshableAsset();
	virtual ~MeshableAsset();
    
    TArray<SubmeshData> submeshes;
};
