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
#pragma once

#include "../Utils/RapidXml.h"
#include "SceneObjects/SceneObjectBase.h"
#include "TextureEntry.h"
#include "../Meshing/PrimMesher.h"
#include "../Meshing/SculptMesher.h"
#include "../AssetSubsystem/AssetBase.h"
#include "../AssetSubsystem/SculptAsset.h"
#include "../AssetSubsystem/MeshAsset.h"


class LLSDItem;

enum PrimFlags : uint32_t
{
    None = 0,
    /// <summary>Whether physics are enabled for this object</summary>
    Physics = 0x00000001,
    /// <summary></summary>
    CreateSelected = 0x00000002,
    /// <summary></summary>
    ObjectModify = 0x00000004,
    /// <summary></summary>
    ObjectCopy = 0x00000008,
    /// <summary></summary>
    ObjectAnyOwner = 0x00000010,
    /// <summary></summary>
    ObjectYouOwner = 0x00000020,
    /// <summary></summary>
    Scripted = 0x00000040,
    /// <summary>Whether this object contains an active touch script</summary>
    Touch = 0x00000080,
    /// <summary></summary>
    ObjectMove = 0x00000100,
    /// <summary>Whether this object can receive payments</summary>
    Money = 0x00000200,
    /// <summary>Whether this object is phantom (no collisions)</summary>
    Phantom = 0x00000400,
    /// <summary></summary>
    InventoryEmpty = 0x00000800,
    /// <summary></summary>
    JointHinge = 0x00001000,
    /// <summary></summary>
    JointP2P = 0x00002000,
    /// <summary></summary>
    JointLP2P = 0x00004000,
    /// <summary>Deprecated</summary>
    JointWheel = 0x00008000,
    /// <summary></summary>
    AllowInventoryDrop = 0x00010000,
    /// <summary></summary>
    ObjectTransfer = 0x00020000,
    /// <summary></summary>
    ObjectGroupOwned = 0x00040000,
    /// <summary>Deprecated</summary>
    ObjectYouOfficer = 0x00080000,
    /// <summary></summary>
    CameraDecoupled = 0x00100000,
    /// <summary></summary>
    AnimSource = 0x00200000,
    /// <summary></summary>
    CameraSource = 0x00400000,
    /// <summary></summary>
    CastShadows = 0x00800000,
    /// <summary>Server flag, will not be sent to clients. Specifies that
    /// the object is destroyed when it touches a simulator edge</summary>
    DieAtEdge = 0x01000000,
    /// <summary>Server flag, will not be sent to clients. Specifies that
    /// the object will be returned to the owner's inventory when it
    /// touches a simulator edge</summary>
    ReturnAtEdge = 0x02000000,
    /// <summary>Server flag, will not be sent to clients.</summary>
    Sandbox = 0x04000000,
    /// <summary>Server flag, will not be sent to client. Specifies that
    /// the object is hovering/flying</summary>
    Flying = 0x08000000,
    /// <summary></summary>
    ObjectOwnerModify = 0x10000000,
    /// <summary></summary>
    TemporaryOnRez = 0x20000000,
    /// <summary></summary>
    Temporary = 0x40000000,
    /// <summary></summary>
    ZlibCompressed = 0x80000000
};

/**
 * 
 */
class AVINATIONVIEWER_API SceneObjectPart : public SceneObjectBase
{
public:
	SceneObjectPart();
	virtual ~SceneObjectPart();
    
    bool Load(rapidxml::xml_node<> *xml);
    void FetchAssets() override;
    bool MeshPrim();
    bool MeshSculpt(TSharedRef<SculptAsset, ESPMode::ThreadSafe> data);
    bool MeshMesh(TSharedRef<MeshAsset, ESPMode::ThreadSafe> data);
    void DeleteMeshData();
    virtual void RequestTextures() override;
    
    virtual SceneObject inline Type() override { return ObjectGroup; }
    FVector groupPosition;
    FVector position;
    FVector scale;
    FQuat rotation;
    FGuid uuid;
    
    TArray<uint8_t> textureEntry;
    TextureEntry defaultTexture;
    TArray<TextureEntry> textures;

    bool isPrim;
    bool isSculpt;
    bool isMesh;

    bool ShouldColide;
    bool isPhantom;
    bool isPhysical;

    int sculptType;
    LevelDetail maxLod = Highest;
    
    FString meshAssetId;
    
    // Prim stuff
    float pathBegin;
    uint8_t pathCurve;
    float pathEnd;
    float pathRadiusOffset;
    float pathRevolutions;
    float pathScaleX;
    float pathScaleY;
    float pathShearX;
    float pathShearY;
    float pathSkew;
    float pathTaperX;
    float pathTaperY;
    float pathTwist;
    float pathTwistBegin;
    float profileBegin;
    float profileEnd;
    float profileHollow;
    ProfileShapeType profileShape;
    HollowShapeType hollowShape;
    uint8_t physicsShapeType;
    int32_t primFlags;

    int numFaces;
    int lodWanted;
    
    float cullDistance;

    int submeshIndex;
    
    PrimMesh *primData;
    SculptMesh *sculptData;
    TArray<PrimFaceMeshData> primMeshData;
    UProceduralMeshComponent *mesh;

private:
    void SculptReceived(FGuid id, TSharedAssetRef asset);
    void MeshReceived(FGuid id, TSharedAssetRef asset);
    float ReadFloatValue(rapidxml::xml_node<> *parent, const char *name, float def);
    int ReadIntValue(rapidxml::xml_node<> *parent, const char *name, int def);
    FString ReadStringValue(rapidxml::xml_node<> *parent, const char *name, FString def);
    void GeneratePrimMesh(int lod);
    void calcVertsNormals(PrimFaceMeshData& pm);
    void calcVertsTangents(PrimFaceMeshData& pm);
    bool meshed = false;
    TArray<uint8_t> meshAssetData;
};
