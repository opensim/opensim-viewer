// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AvinationBaseActor.h"
#include "Http.h"
#include "SceneObjects/SceneObjectGroup.h"
#include "SceneObjects/SceneObjectPart.h"
#include "ProceduralMeshComponent.h"
#include "MeshActor.generated.h"
class LLSDItem;
class AMeshActor;

DECLARE_DELEGATE_OneParam(ObjectReadyDelegate, AMeshActor *)

UCLASS()
class AVINATIONVIEWER_API AMeshActor : public AAvinationBaseActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMeshActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;
    bool Load(rapidxml::xml_node<> *data);
    void RegisterComponents();
    void GotTexture(FGuid id, TSharedAssetRef asset, UProceduralMeshComponent *mesh, int index, TextureEntry *te);
    SceneObjectGroup *sog;
    ObjectReadyDelegate OnObjectReady;

//    bool ShouldCollide;
    
private:
    UPROPERTY()
    UMaterial *baseMaterial;
    UPROPERTY()
    UMaterial *baseMaterialTranslucent;

    UProceduralMeshComponent *BuildComponent(SceneObjectPart *sop);
    
    void ObjectReady();
    UMaterialInstanceDynamic *SetUpMaterial(UProceduralMeshComponent *mesh, int textureIndex, UMaterial *baseMaterial, TextureEntry& te);
};
