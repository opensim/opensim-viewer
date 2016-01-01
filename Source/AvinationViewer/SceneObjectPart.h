// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "RapidXml.h"
#include "SceneObjectBase.h"
#include "TextureEntry.h"
#include "PrimMesher.h"
#include "SculptMesher.h"

class LLSDItem;

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
    bool MeshSculpt(TArray<uint8_t> data);
    
    virtual bool inline IsObjectPart() override { return true; }
    LLSDItem * GetMeshData();
    
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
    
    int sculptType;
    
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
    uint8_t profileShape;
    uint8_t hollowShape;
    
    int numFaces;
    
    PrimMesh *primData;
    SculptMesh *sculptData;
private:
    void AssetReceived(FGuid id, int status, TArray<uint8_t> data) override;
    float ReadFloatValue(rapidxml::xml_node<> *parent, const char *name, float def);
    int ReadIntValue(rapidxml::xml_node<> *parent, const char *name, int def);
    FString ReadStringValue(rapidxml::xml_node<> *parent, const char *name, FString def);
    void GeneratePrimMesh(int lod);
    void GenerateSculptMesh(TArray<uint8_t> data);
    
    bool meshed = false;
    TArray<uint8_t> meshAssetData;
};
