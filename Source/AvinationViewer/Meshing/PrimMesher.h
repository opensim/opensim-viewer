// Fill out your copyright notice in the Description page of Project Settings.
#include "ProceduralMeshComponent.h"
#pragma once
#define _USE_MATH_DEFINES
#include <math.h>
#include "Coord.h"

enum PathType { Linear = 0, Circular = 1, Flexible = 2 };

enum ProfileShapeType : uint8_t
{
    pstCircle = 0,
    pstSquare = 1,
    pstIsometricTriangle = 2,
    pstEquilateralTriangle = 3,
    pstRightTriangle = 4,
    pstHalfCircle = 5
};

enum HollowShapeType : uint8_t
{
    hstSame = 0,
    hstCircle = 16,
    hstSquare = 32,
    hstTriangle = 48
};

enum ExtrusionType : uint8_t
{
    etStraight = 0x10,
    etCurve1 = 0x20,
    etCurve2 = 0x30,
    etFlexible = 0x80
};

enum LevelDetail
{
    Lowest  = 0,
    Low     = 1,
    High    = 2,
    Highest = 3   
};

/**
 * 
 */

class Face
{
public:
    Face();
    Face(int v1, int v2, int v3);
    
    FVector SurfaceNormal(TArray<FVector> coordList);

    int primFace;
    
    // vertices
    int v1;
    int v2;
    int v3;
    
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
    void AddRot(FQuat q);
    int primFaceNumber;
    
    FVector v1;
    FVector v2;
    FVector v3;
    
    int coordIndex1;
    int coordIndex2;
    int coordIndex3;
    
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
    static const Angle angles4[];
    static const Angle angles24[];
    
    TArray<Angle> angles;
    
    void makeAngles(int sides, float startAngle, float stopAngle);
    
private:
    Angle interpolatePoints(float newPoint, Angle p1, Angle p2);
    void intersection(double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4);
};

struct PathNode
{
    FVector position;
    FQuat rotation;
    float xScale;
    float yScale;
    float percentOfPath;
};

class Profile
{
public:
    const float twoPi = M_PI * 2;
    
    FString errorMessage;
    
    TArray<FVector> coords;
    TArray<Face> faces;
    TArray<float> us;
    TArray<UVCoord> faceUVs;
    TArray<int> faceNumbers;
    
    // use these for making individual meshes for each prim face
    TArray<int> outerCoordIndices;
    TArray<int> hollowCoordIndices;
    TArray<int> cut1CoordIndices;
    TArray<int> cut2CoordIndices;
    
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
    void AddPos(FVector v);
    void AddPos(float x, float y, float z);
    void AddRot(FQuat q);
    void Scale(float x, float y);
    void FlipNormals();
    void AddValue2FaceVertexIndices(int num);
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
    
    TArray<FVector> coords;
    TArray<FVector> normals;
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
    
    
    FVector SurfaceNormal(FVector c1, FVector c2, FVector c3);
    FVector SurfaceNormal(Face face);
    FVector SurfaceNormal(int faceIndex);
    PrimMesh Copy();
    void AddPos(float x, float y, float z);
    void AddRot(FQuat q);
    
#if 0
    VertexIndexer GetVertexIndexer();
#endif
    void Scale(float x, float y, float z);
    void DumpRaw(FString path, FString name, FString title);
};

struct PrimFaceMeshData
{
    TArray<FVector> vertices;
    TArray<int32> triangles;
    TArray<FVector> normals;
    TArray<FVector2D> uv0;
    TArray<FColor> vertexColors;
    TArray<FProcMeshTangent> tangents;
};
