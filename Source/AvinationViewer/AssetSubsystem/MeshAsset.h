// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "MeshableAsset.h"

/**
 * 
 */
class AVINATIONVIEWER_API MeshAsset : public MeshableAsset
{
public:
	MeshAsset();
	virtual ~MeshAsset();
    
    TArray<uint8_t> meshData;
    
private:
    void Process();
};
