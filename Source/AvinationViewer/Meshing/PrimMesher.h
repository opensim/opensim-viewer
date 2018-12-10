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
 */#include "ProceduralMeshComponent.h"
#pragma once
#define _USE_MATH_DEFINES
#include <math.h>

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
    
    FVector2D uv1;
    FVector2D uv2;
    FVector2D uv3;
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
    TArray<FVector2D> faceUVs;
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
    void FlipUV(FVector2D& uv);
    
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
