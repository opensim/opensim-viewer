// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "MeshableAsset.h"
#include "../Meshing/PrimMesher.h"
#include "../AssetSubsystem/LLSDMeshDecode.h"
#include "../Meshing/PrimMesher.h"

/**
 * 
 */
class AVINATIONVIEWER_API MeshAsset : public MeshableAsset
{
public:
	MeshAsset();
	virtual ~MeshAsset();
    
    TArray<uint8_t> meshData;
    TArray<SubmeshData> lodMeshs;
    
private:
    void Process();
    LLSDItem* GetMeshData(int lod);
    bool MeshMesh(SubmeshData& meshData, int lod);
    void calcVertsTangents(PrimFaceMeshData& pm); // needs to be moved to a shared location
};
