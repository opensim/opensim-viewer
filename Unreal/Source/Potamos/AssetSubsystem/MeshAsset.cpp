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
#include "Potamos.h"
#include "../Utils/AvinationUtils.h"
#include "MeshAsset.h"


MeshAsset::MeshAsset()
{
    mainProcess.BindRaw(this, &MeshAsset::Process);
}

MeshAsset::~MeshAsset()
{
}

void MeshAsset::Process()
{
    meshData = TArray<uint8_t>(*stageData);
    stageData.Reset();

    lodMeshs.Empty();

    for (int i = 3; i >= 0; i--)
    {
        SubmeshData lodMesh;
        if(!MeshMesh(lodMesh, i))
            break;
        lodMeshs.Add(lodMesh);
    }

    meshData.Empty();
}

LLSDItem *MeshAsset::GetMeshData(int lod)
{
    FString lodLevel;
    switch ((LevelDetail)lod)
    {
    case Highest:
        lodLevel = TEXT("high_lod");
        break;
    case High:
        lodLevel = TEXT("medium_lod");
        break;
    case Low:
        lodLevel = TEXT("low_lod");
        break;
    default:
        lodLevel = TEXT("lowest_lod");
        break;
    }

    LLSDItem *lodData = LLSDMeshDecode::Decode(meshData.GetData(), lodLevel);
    return lodData;
}

bool MeshAsset::MeshMesh(SubmeshData& subMesh, int lod)
{
    LLSDItem* data = GetMeshData(lod);
    if (!data)
        return false;

    if (data->arrayData.Num() == 0)
        return false;

    int textureIndex = 0;
    int numFaces = 0;
    
    subMesh.meshFaces.Empty();

    for (auto face = data->arrayData.CreateConstIterator(); face; ++face)
    {
        if ((*face)->mapData.Num() == 0)
            continue;

        if ((*face)->mapData.Contains(TEXT("NoGeometry")))
            continue;

        if (!(*face)->mapData.Contains(TEXT("Position")))
            continue;

        PrimFaceMeshData pm;

        float minX = -0.5f, minY = -0.5f, minZ = -0.5f, maxX = 0.5f, maxY = 0.5f, maxZ = 0.5f;
        float minU = 0.0f, minV = 0.0f, maxU = 0.0f, maxV = 0.0f;

        if ((*face)->mapData.Contains(TEXT("PositionDomain")))
        {
            //LLSDDecode::DumpItem((*face)->mapData[TEXT("PositionDomain")]);
            LLSDItem *positionDomain = (*face)->mapData[TEXT("PositionDomain")];

            LLSDItem *min = positionDomain->mapData[TEXT("Min")];
            LLSDItem *max = positionDomain->mapData[TEXT("Max")];

            minX = (float)(min->arrayData[0]->data.doubleData);
            minY = (float)(min->arrayData[1]->data.doubleData);
            minZ = (float)(min->arrayData[2]->data.doubleData);

            maxX = (float)(max->arrayData[0]->data.doubleData);
            maxY = (float)(max->arrayData[1]->data.doubleData);
            maxZ = (float)(max->arrayData[2]->data.doubleData);
        }

        if ((*face)->mapData.Contains(TEXT("TexCoord0Domain")))
        {
            LLSDItem *texDomain = (*face)->mapData[TEXT("TexCoord0Domain")];

            LLSDItem *min = texDomain->mapData[TEXT("Min")];
            LLSDItem *max = texDomain->mapData[TEXT("Max")];

            minU = (float)(min->arrayData[0]->data.doubleData);
            minV = (float)(min->arrayData[1]->data.doubleData);

            maxU = (float)(max->arrayData[0]->data.doubleData);
            maxV = (float)(max->arrayData[1]->data.doubleData);
        }

        //UE_LOG(LogTemp, Warning, TEXT("Face %d PositionDomain %f, %f, %f - %f, %f, %f"), textureIndex, minX, minY, minZ, maxX, maxY, maxZ);

        int binaryLength = (*face)->mapData[TEXT("Position")]->binaryLength;

        int numVertices = binaryLength / 6; // 3 x uint16_t
        uint16_t *vertexData = (uint16_t *)((*face)->mapData[TEXT("Position")]->data.binaryData);

        //UE_LOG(LogTemp, Warning, TEXT("Vertex count %d Normals count %d"), numVertices, numNormals);

        for (int idx = 0; idx < numVertices; ++idx)
        {
            uint16_t posX = *vertexData++;
            uint16_t posY = *vertexData++;
            uint16_t posZ = *vertexData++;

            FVector pos(AvinationUtils::uint16tofloat(posX, minX, maxX),
                -AvinationUtils::uint16tofloat(posY, minY, maxY),
                AvinationUtils::uint16tofloat(posZ, minZ, maxZ));
            pm.vertices.Add(pos);

            //UE_LOG(LogTemp, Warning, TEXT("Vertex %s"), *pos.ToString());

            pm.tangents.Add(FProcMeshTangent(1, 1, 1));
            pm.vertexColors.Add(FColor(255, 255, 255, 255));
        }
        if ((*face)->mapData.Contains(TEXT("Normal")))
        {
            int normalsLength = (*face)->mapData[TEXT("Normal")]->binaryLength;
            int numNormals = binaryLength / 6; // 3 x uint16_t
            if (numNormals > numVertices)
                numNormals = numVertices;
            uint16_t *normalsData = (uint16_t *)((*face)->mapData[TEXT("Normal")]->data.binaryData);
            for (int idx = 0; idx < numNormals; ++idx)
            {
                uint16_t norX = *normalsData++;
                uint16_t norY = *normalsData++;
                uint16_t norZ = *normalsData++;
                FVector nor(AvinationUtils::uint16tofloat(norX, -1.0f, 1.0f),
                    -AvinationUtils::uint16tofloat(norY, -1.0f, 1.0f),
                    AvinationUtils::uint16tofloat(norZ, -1.0f, 1.0f));
                pm.normals.Add(nor);
            }
        }

        uint16_t numTriangles = (*face)->mapData[TEXT("TriangleList")]->binaryLength / 6; // 3 * uint16_t
        uint16_t *trianglesData = (uint16_t *)((*face)->mapData[TEXT("TriangleList")]->data.binaryData);

        //UE_LOG(LogTemp, Warning, TEXT("Triangles count %d"), numTriangles);
        for (int idx = 0; idx < numTriangles; ++idx)
        {
            uint16_t t1 = *trianglesData++;
            uint16_t t2 = *trianglesData++;
            uint16_t t3 = *trianglesData++;
            pm.triangles.Add(t1);
            pm.triangles.Add(t2);
            pm.triangles.Add(t3);
        }

        uint16_t numUvs = (*face)->mapData[TEXT("TexCoord0")]->binaryLength / 4; // 3 * uint16_t
        uint16_t *uvData = (uint16_t *)((*face)->mapData[TEXT("TexCoord0")]->data.binaryData);

        //UE_LOG(LogTemp, Warning, TEXT("UV count %d"), numUvs);

        for (int idx = 0; idx < numUvs; ++idx)
        {
            uint16_t u = *uvData++;
            uint16_t v = *uvData++;

            pm.uv0.Add(FVector2D(AvinationUtils::uint16tofloat(u, minU, maxU),
                1.0 - AvinationUtils::uint16tofloat(v, minV, maxV)));
        }
        calcVertsTangents(pm);
        subMesh.meshFaces.Add(pm);
        numFaces++;
    }

    delete data;

    subMesh.numFaces = numFaces;
    if (numFaces == 0)
        return false;

    return true;
}

// this must be moved to a shared location/class
void MeshAsset::calcVertsTangents(PrimFaceMeshData& pm)
{
    int numverts = pm.vertices.Num();
    if (numverts == 0)
        return;
    int numtris = pm.triangles.Num();
    if (numtris == 0)
        return;

    pm.tangents.Empty();
    pm.tangents.AddZeroed(numverts);

    TArray<FVector> tan1;
    TArray<FVector> tan2;
    tan1.AddZeroed(numverts);
    tan2.AddZeroed(numverts);

    for (int a = 0; a < numtris;)
    {
        int i1 = pm.triangles[a++];
        int i2 = pm.triangles[a++];
        int i3 = pm.triangles[a++];

        const FVector v1 = pm.vertices[i1];
        const FVector v2 = pm.vertices[i2];
        const FVector v3 = pm.vertices[i3];

        const FVector2D w1 = pm.uv0[i1];
        const FVector2D w2 = pm.uv0[i2];
        const FVector2D w3 = pm.uv0[i3];

        float x1 = v2.X - v1.X;
        float x2 = v3.X - v1.X;
        float y1 = v2.Y - v1.Y;
        float y2 = v3.Y - v1.Y;
        float z1 = v2.Z - v1.Z;
        float z2 = v3.Z - v1.Z;

        float s1 = w2.X - w1.X;
        float s2 = w3.X - w1.X;
        float t1 = w2.Y - w1.Y;
        float t2 = w3.Y - w1.Y;

        float r = s1 * t2 - s2 * t1;
        r = (fabs(r) > 1e-5) ? 1.0f / r : (r > 0.0 ? 1.0e3 : 1.0e3);

        FVector sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r,
            (t2 * z1 - t1 * z2) * r);
        FVector tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r,
            (s1 * z2 - s2 * z1) * r);

        tan1[i1] += sdir;
        tan1[i2] += sdir;
        tan1[i3] += sdir;

        tan2[i1] += tdir;
        tan2[i2] += tdir;
        tan2[i3] += tdir;
    }

    for (int a = 0; a < numverts; a++)
    {
        FVector n = pm.normals[a];
        FVector t = tan1[a];

        float dotnt = FVector::DotProduct(n, t);
        FVector crossnt = FVector::CrossProduct(n, t);

        FVector tsubn = t - n * crossnt;

        if (tsubn.SizeSquared() > 1e-5)
        {
            tsubn.Normalize();
            float dotCrossT2 = FVector::DotProduct(crossnt, tan2[a]);

            pm.tangents[a] = FProcMeshTangent(FVector(-tsubn.X, tsubn.Y, -tsubn.Z), (dotCrossT2 < 0.0F));
        }
        else
            pm.tangents[a] = FProcMeshTangent(FVector(0.0, 0.0, -1), true);
    }
}


