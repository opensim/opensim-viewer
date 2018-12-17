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

#include "PotBaseActor.h"
#include "Http.h"
#include "SceneObjects/SceneObjectGroup.h"
#include "SceneObjects/SceneObjectPart.h"
#include "ProceduralMeshComponent.h"
#include "MeshActor.generated.h"
class LLSDItem;
class AMeshActor;

DECLARE_DELEGATE_OneParam(ObjectReadyDelegate, AMeshActor *)

UCLASS()
class POTAMOS_API AMeshActor : public APotBaseActor
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
    UPROPERTY()
        UMaterial *baseMaterialUnLit;
    UPROPERTY()
        UMaterial *baseMaterialTranslucentUnLit;

    UProceduralMeshComponent *BuildComponent(SceneObjectPart *sop);
    
    void ObjectReady();
    UMaterialInstanceDynamic *SetUpMaterial(UProceduralMeshComponent *mesh, int textureIndex, UMaterial *baseMaterial, TextureEntry& te);
    UMaterialInstanceDynamic *SetUpMaterialUnLit(UProceduralMeshComponent *mesh, int textureIndex, UMaterial *baseMaterial, TextureEntry& te);
};
