// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Coord.h"
#include "PrimMesher.h"

/**
 * 
 */
enum SculptType { sphere = 1, torus = 2, plane = 3, cylinder = 4 };

class AVINATIONVIEWER_API SculptMesh
{
public:
    TArray<FVector> coords;
    TArray<Face> faces;
    TArray<FVector> normals;
    TArray<UVCoord> uvs;
    
    SculptMesh(TArray<TArray<FVector>>& rows, SculptType sculptType, bool viewerMode, bool mirror, bool invert, int lod);

private:
    void calcVertexNormals(SculptType sculptType, int xSize, int ySize);
};