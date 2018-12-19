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
#include "MeshActor.h"
#include "AssetSubsystem/LLSDItem.h"
#include "AssetSubsystem/LLSDDecode.h"
#include "Utils/AvinationUtils.h"
#include "AssetSubsystem/AssetCache.h"
#include "AssetSubsystem/AssetDecode.h"
#include "AssetSubsystem/TextureAsset.h"
#include "SceneObjects/TextureEntry.h"
#include "Meshing/PrimMesher.h"

#include <stdlib.h>
#include <fcntl.h>
//#include <unistd.h>
#include <stdio.h>

// Sets default values
AMeshActor::AMeshActor()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;
    
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
    
    ConstructorHelpers::FObjectFinder<UMaterial> parentMaterial(TEXT("Material'/Game/Materials/DynamicBase.DynamicBase'"));

    baseMaterial = parentMaterial.Object;

    ConstructorHelpers::FObjectFinder<UMaterial> parentMaterialTranslucent(TEXT("Material'/Game/Materials/DynamicBaseTranslucent.DynamicBaseTranslucent'"));

    baseMaterialTranslucent = parentMaterialTranslucent.Object;

    ConstructorHelpers::FObjectFinder<UMaterial> parentMaterialUnLit(TEXT("Material'/Game/Materials/DynamicBaseUnLit.DynamicBaseUnLit'"));

    baseMaterialUnLit = parentMaterialUnLit.Object;

    ConstructorHelpers::FObjectFinder<UMaterial> parentMaterialTranslucentUnLit(TEXT("Material'/Game/Materials/DynamicBaseTranslucentUnLit.DynamicBaseTranslucentUnLit'"));

    baseMaterialTranslucentUnLit = parentMaterialTranslucentUnLit.Object;
}

void AMeshActor::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AMeshActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AMeshActor::Load(rapidxml::xml_node<> *data)
{
    sog = new SceneObjectGroup();
    sog->actor = this;
    
    if (sog->Load(data))
    {
        sog->OnObjectReady.BindUObject(this, &AMeshActor::ObjectReady);
        
        sog->FetchAssets();
    }
    
    return true;
}

void AMeshActor::ObjectReady()
{
    OnObjectReady.ExecuteIfBound(this);
}

void AMeshActor::BuildObject()
{
	ClearInstanceComponents(true);

	USceneComponent *rootComponent = nullptr;

	SceneObjectPart *sop = sog->GetRootPart();
	FString rootName = TEXT("r-");
	rootName += *sop->uuid.ToString();

	// create a component that will hold parts, and other action components
	// its scale will only be changed on operations that scale all sog
	// its position  and rotation is the sog one
	rootComponent = NewObject<USceneComponent>(this, *rootName);
	SetRootComponent(rootComponent);

	bool ShouldCollide = !sop->isPhantom;

	// sog absolute position and rotation from root part
	FVector p = sop->position * 100;
	RootComponent->SetWorldLocationAndRotation(p, sop->rotation);
	RootComponent->SetWorldScale3D(FVector(1.0f));

	UProceduralMeshComponent *mesh = BuildComponent(sop);
	sop->DeleteMeshData();
	bool notphysical = !sop->isPhysical;

	mesh->AttachTo(RootComponent);
	// root part relative position and rotation should always be null
	// unless explicitly changed (as in edit parts on root prim)
	mesh->SetRelativeLocation(FVector(0.0f));
	mesh->SetWorldScale3D(sog->GetRootPart()->scale);

	if (notphysical)
	{
		mesh->Mobility = EComponentMobility::Static;
	}
	//    if (ShouldCollide)
//    {
//        mesh->SetCollisionProfileName(TEXT("WorldStatic"));
//        mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
//    }
	int index = 0;

	TArray<SceneObjectBase *> parts = sog->GetParts();

	for (auto it = parts.CreateConstIterator(); it; ++it)
	{
		sop = (SceneObjectPart *)(*it);

		// Skip root part
		if (index > 0)
		{
			UProceduralMeshComponent *subMesh = BuildComponent(sop);
			sop->DeleteMeshData();

			//subMesh->AttachParent = mesh;
			subMesh->AttachTo(RootComponent);
			subMesh->SetWorldScale3D(sop->scale);
			p = sop->position * 100;
			subMesh->SetRelativeLocationAndRotation(p, sop->rotation);
			//            if (ShouldCollide && sop->ShouldColide)
			//            {
			//                mesh->SetCollisionProfileName(TEXT("WorldStatic"));
			//                mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			//            }
			if (notphysical)
			{
				mesh->Mobility = EComponentMobility::Static;

			}
		}

		++index;
	}
}

void AMeshActor::RegisterComponents()
{
    for (auto it = GetComponents().CreateConstIterator() ; it ; ++it)
    {
        (*it)->RegisterComponent();
    }
}

UProceduralMeshComponent *AMeshActor::BuildComponent(SceneObjectPart *sop)
{
    UProceduralMeshComponent *mesh = NewObject<UProceduralMeshComponent>(this, *sop->uuid.ToString());

    sop->mesh = mesh;
    
    mesh->Mobility = EComponentMobility::Stationary;
    //mesh->RegisterComponent();
    mesh->bAutoRegister = false;
    mesh->CastShadow = true;
    mesh->SetCullDistance(sop->cullDistance);
    mesh->SetCachedMaxDrawDistance(sop->cullDistance);

    int nfaces = sop->primMeshData.Num();
    if (nfaces == 0)
        return mesh;
    
    int primFace = 0;
    for (int srcFace = 0; srcFace < nfaces; srcFace++)
    {
        PrimFaceMeshData *face = &sop->primMeshData[srcFace];
        if (face->vertices.Num() == 0 || face->triangles.Num() == 0)
            continue;
        mesh->CreateMeshSection(primFace, face->vertices, face->triangles,
            face->normals, face->uv0, face->vertexColors, face->tangents, false);
        primFace++;
    }

    if (sop->primData)
    {
        delete sop->primData;
        sop->primData = 0;
    }
    sop->primMeshData.Empty();
    
    return mesh;
}

UMaterialInstanceDynamic *AMeshActor::SetUpMaterial(UProceduralMeshComponent *mesh, int textureIndex, UMaterial *bMat, TextureEntry& te)
{
    //if (baseMat == baseMaterialTranslucent)
    //    UE_LOG(LogTemp, Warning, TEXT("Translucent texture"));

    mesh->SetMaterial(textureIndex, bMat);
    UMaterialInstanceDynamic *mat = mesh->CreateAndSetMaterialInstanceDynamic(textureIndex);

    float repeatU = te.repeatU;
    float repeatV = te.repeatV;

    float offsetU = 0.5 * (1 - repeatU) + te.offsetU;
    float offsetV = 0.5 * (1 - repeatV) - te.offsetV;

    mat->SetVectorParameterValue(TEXT("Color"), te.color);
    mat->SetVectorParameterValue(TEXT("Offset"), FLinearColor(offsetU, offsetV, 0.0f, 0.0f));
    mat->SetVectorParameterValue(TEXT("Repeats"), FLinearColor(repeatU, repeatV, 0.0f, 0.0f));
    mat->SetScalarParameterValue(TEXT("Rotation"), te.rotation);

    return mat;
}

UMaterialInstanceDynamic *AMeshActor::SetUpMaterialUnLit(UProceduralMeshComponent *mesh, int textureIndex, UMaterial *bMat, TextureEntry& te)
{
    //if (baseMat == baseMaterialTranslucent)
    //    UE_LOG(LogTemp, Warning, TEXT("Translucent texture"));

    mesh->SetMaterial(textureIndex, bMat);
    UMaterialInstanceDynamic *mat = mesh->CreateAndSetMaterialInstanceDynamic(textureIndex);

    float repeatU = te.repeatU;
    float repeatV = te.repeatV;

    float offsetU = 0.5 * (1 - repeatU) + te.offsetU;
    float offsetV = 0.5 * (1 - repeatV) - te.offsetV;

    float emissive = 3.0f * te.glow + 0.7f; // values to tune
    mat->SetScalarParameterValue(TEXT("Emissive"), emissive);
    mat->SetVectorParameterValue(TEXT("Color"), te.color);
    mat->SetVectorParameterValue(TEXT("Offset"), FLinearColor(offsetU, offsetV, 0.0f, 0.0f));
    mat->SetVectorParameterValue(TEXT("Repeats"), FLinearColor(repeatU, repeatV, 0.0f, 0.0f));
    mat->SetScalarParameterValue(TEXT("Rotation"), te.rotation);


    return mat;
}

void AMeshActor::GotTexture(FGuid id, TSharedAssetRef asset, UProceduralMeshComponent *mesh, int index, TextureEntry *te)
{
    TSharedRef<TextureAsset, ESPMode::ThreadSafe> t = StaticCastSharedRef<TextureAsset>(asset);

    UMaterialInstanceDynamic* mat;
    bool hasAlpha = (t->hasAlpha || te->color.A < 0.99f);
    bool emissive = (te->material & 0x20) || te->glow > 0.01;

    if (emissive)
    {
        if (hasAlpha)
            mat = SetUpMaterialUnLit(mesh, index, baseMaterialTranslucentUnLit, *te);
        else
            mat = SetUpMaterialUnLit(mesh, index, baseMaterialUnLit, *te);
    }
    else
    {
        if (hasAlpha)
            mat = SetUpMaterial(mesh, index, baseMaterialTranslucent, *te);
        else
            mat = SetUpMaterial(mesh, index, baseMaterial, *te);
    }

    mat->SetTextureParameterValue(TEXT("Texture"), t->tex);
}
