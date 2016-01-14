// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "ProceduralMeshComponent.h"
#include "../Meshing/PrimMesher.h"

/**
 * 
 */
class AVINATIONVIEWER_API SubmeshData
{
public:
	SubmeshData();
	~SubmeshData();
    
    TArray<PrimFaceMeshData> meshFaces;
    int numFaces = 0;
};
