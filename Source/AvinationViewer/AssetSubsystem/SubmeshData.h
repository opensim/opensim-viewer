// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "ProceduralMeshComponent.h"

/**
 * 
 */
class AVINATIONVIEWER_API SubmeshData
{
public:
	SubmeshData();
	~SubmeshData();
    
    TArray<FVector> vertices;
    TArray<int32> triangles;
    TArray<FVector> normals;
    TArray<FVector2D> uv0;
    TArray<FColor> vertexColors;
    TArray<FProcMeshTangent> tangents;
};
