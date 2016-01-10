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
    TArray<Coord> coords;
    TArray<Face> faces;
    
    TArray<ViewerFace> viewerFaces;
    TArray<Coord> normals;
    TArray<UVCoord> uvs;
    TArray<Coord> tangents;
    
    SculptMesh(TArray<TArray<Coord>> rows, SculptType sculptType, bool viewerMode, bool mirror, bool invert, int lod);
    
    void _SculptMesh(TArray<TArray<Coord>> rows, SculptType sculptType, bool viewerMode, bool mirror, bool invert, int lod);
    
    void AddPos(float x, float y, float z);
    void AddRot(PrimQuat q);
    void Scale(float x, float y, float z);
private:
    void calcVertexNormals(SculptType sculptType, int xSize, int ySize);
    void CalcTangents();
};