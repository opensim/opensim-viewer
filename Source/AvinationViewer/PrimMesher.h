// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#define _USE_MATH_DEFINES
#include "Coord.h"
#include "PrimQuat.h"
#include <math.h>

enum PathType { Linear = 0, Circular = 1, Flexible = 2 };

/**
 * 
 */

class Face
{
public:
    Face();
    Face(int v1, int v2, int v3);
    Face(int v1, int v2, int v3, int n1, int n2, int n3);
    
    Coord SurfaceNormal(TArray<Coord> coordList);
    
    int primFace;
    
    // vertices
    int v1;
    int v2;
    int v3;
    
    //normals
    int n1;
    int n2;
    int n3;
    
    // uvs
    int uv1;
    int uv2;
    int uv3;
};

class ViewerFace
{
public:
    ViewerFace(int primFaceNumber);
    
    void Scale(float x, float y, float z);
    void AddPos(float x, float y, float z);
    void AddRot(PrimQuat q);
    void CalcSurfaceNormal();
    int primFaceNumber;
    
    Coord v1;
    Coord v2;
    Coord v3;
    
    int coordIndex1;
    int coordIndex2;
    int coordIndex3;
    
    Coord n1;
    Coord n2;
    Coord n3;
    
    UVCoord uv1;
    UVCoord uv2;
    UVCoord uv3;
};

class Angle
{
public:
    float angle;
    float X;
    float Y;
    
    Angle();
    Angle(float angle, float x, float y);
};

class AngleList
{
public:
    float iX, iY; // intersection point
    
    static const Angle angles3[];
    static const Coord normals3[];
    
    static const Angle angles4[];
    static const Coord normals4[];
    static const Angle angles24[];
    
    
    TArray<Angle> angles;
    TArray<Coord> normals;
    
    void makeAngles(int sides, float startAngle, float stopAngle);
    
private:
    Angle interpolatePoints(float newPoint, Angle p1, Angle p2);
    void intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
};

struct PathNode
{
    Coord position;
    PrimQuat rotation;
    float xScale;
    float yScale;
    float percentOfPath;
};

class Profile
{
public:
    const float twoPi = M_PI * 2;
    
    FString errorMessage;
    
    TArray<Coord> coords;
    TArray<Face> faces;
    TArray<Coord> vertexNormals;
    TArray<float> us;
    TArray<UVCoord> faceUVs;
    TArray<int> faceNumbers;
    
    // use these for making individual meshes for each prim face
    TArray<int> outerCoordIndices;
    TArray<int> hollowCoordIndices;
    TArray<int> cut1CoordIndices;
    TArray<int> cut2CoordIndices;
    
    Coord faceNormal;
    Coord cutNormal1;
    Coord cutNormal2;
    
    int numOuterVerts;
    int numHollowVerts;
    
    int outerFaceNumber;
    int hollowFaceNumber;
    
    bool calcVertexNormals = false;
    int bottomFaceNumber = 0;
    int numPrimFaces = 0;
    
    Profile();
    Profile(int sides, float profileStart, float profileEnd, float hollow, int hollowSides, bool createFaces, bool calcVertexNormals);
    
    void MakeFaceUVs();
    Profile Copy();
    Profile Copy(bool needFaces);
    void AddPos(Coord v);
    void AddPos(float x, float y, float z);
    void AddRot(PrimQuat q);
    void Scale(float x, float y);
    void FlipNormals();
    void AddValue2FaceVertexIndices(int num);
    void AddValue2FaceNormalIndices(int num);
    void DumpRaw(FString path, FString name, FString title);
};

class Path
{
public:
    TArray<PathNode> pathNodes;
    
    float twistBegin = 0.0f;
    float twistEnd = 0.0f;
    float topShearX = 0.0f;
    float topShearY = 0.0f;
    float pathCutBegin = 0.0f;
    float pathCutEnd = 1.0f;
    float dimpleBegin = 0.0f;
    float dimpleEnd = 1.0f;
    float skew = 0.0f;
    float holeSizeX = 1.0f; // called pathScaleX in pbs
    float holeSizeY = 0.25f;
    float taperX = 0.0f;
    float taperY = 0.0f;
    float radius = 0.0f;
    float revolutions = 1.0f;
    int stepsPerRevolution = 24;
    
    const float twoPi = 2.0f * M_PI;
    
    void Create(PathType pathType, int steps);
};

class PrimMesh
{
public:
    FString errorMessage = "";
    const float twoPi = 2.0f * (float)M_PI;
    
    TArray<Coord> coords;
    TArray<Coord> normals;
    TArray<Face> faces;
    
    TArray<ViewerFace> viewerFaces;
    
    int sides = 4;
    int hollowSides = 4;
    float profileStart = 0.0f;
    float profileEnd = 1.0f;
    float hollow = 0.0f;
    int twistBegin = 0;
    int twistEnd = 0;
    float topShearX = 0.0f;
    float topShearY = 0.0f;
    float pathCutBegin = 0.0f;
    float pathCutEnd = 1.0f;
    float dimpleBegin = 0.0f;
    float dimpleEnd = 1.0f;
    float skew = 0.0f;
    float holeSizeX = 1.0f; // called pathScaleX in pbs
    float holeSizeY = 0.25f;
    float taperX = 0.0f;
    float taperY = 0.0f;
    float radius = 0.0f;
    float revolutions = 1.0f;
    int stepsPerRevolution = 24;
    
    int profileOuterFaceNumber = -1;
    int profileHollowFaceNumber = -1;
    
    bool hasProfileCut = false;
    bool hasHollow = false;
    bool calcVertexNormals = false;
    bool normalsProcessed = false;
    bool viewerMode = false;
    bool sphereMode = false;
    
    int numPrimFaces = 0;
    
    FString ParamsToDisplayString();
    
    PrimMesh(int sides, float profileStart, float profileEnd, float hollow, int hollowSides);
    void Extrude(PathType pathType);
    inline void ExtrudeLinear() { Extrude(Linear); }
    inline void ExtrudeCircular() { Extrude(Circular); }
    
    
    Coord SurfaceNormal(Coord c1, Coord c2, Coord c3);
    Coord SurfaceNormal(Face face);
    Coord SurfaceNormal(int faceIndex);
    PrimMesh Copy();
    void CalcNormals();
    void AddPos(float x, float y, float z);
    void AddRot(PrimQuat q);
    
#if 0
    VertexIndexer GetVertexIndexer();
#endif
    void Scale(float x, float y, float z);
    void DumpRaw(FString path, FString name, FString title);
};
