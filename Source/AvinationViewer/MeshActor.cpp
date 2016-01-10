// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "MeshActor.h"
#include "AssetSubsystem/LLSDItem.h"
#include "AssetSubsystem/LLSDDecode.h"
#include "Utils/AvinationUtils.h"
#include "AssetSubsystem/AssetCache.h"
#include "AssetSubsystem/AssetDecode.h"
#include "SceneObjects/TextureEntry.h"
#include "AssetSubsystem/TextureCache.h"
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
}

// Called when the game starts or when spawned
void AMeshActor::BeginPlay()
{
    SetActorHiddenInGame(true);
    SetActorEnableCollision(false);
//	Super::BeginPlay(); // this needs to be called from mainthread 
	
}

void AMeshActor::DoBeginPlay()
{
    Super::BeginPlay();
//    if(ShouldCollide)
//        SetActorEnableCollision(true);
}

// Called every frame
void AMeshActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

bool AMeshActor::Load(rapidxml::xml_node<> *data)
{
    sog = new SceneObjectGroup();
    if (sog->Load(data))
    {
        sog->OnObjectReady.BindUObject(this, &AMeshActor::ObjectReady);
        
        sog->FetchAssets();
    }
    
    return true;
}

void AMeshActor::ObjectReady()
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
    
    for (auto it = parts.CreateConstIterator() ; it ; ++it)
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
    
    OnObjectReady.ExecuteIfBound(this);
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
    UProceduralMeshComponent *mesh;
    
//    if (sop->isPrim || sop->isMesh || sop->isSculpt)
    {
        mesh = BuildComponentFromPrimData(sop);
    }
    
    return mesh;
}

UProceduralMeshComponent *AMeshActor::BuildComponentFromPrimData(SceneObjectPart *sop)
{
    UProceduralMeshComponent *mesh = NewObject<UProceduralMeshComponent>(this, *sop->uuid.ToString());
    
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
        PrimFaceMeshData face = sop->primMeshData[srcFace];
        if (face.vertices.Num() == 0 || face.triangles.Num() == 0)
            continue;
        mesh->CreateMeshSection(primFace, face.vertices, face.triangles,
            face.normals, face.uv0, face.vertexColors, face.tangents, false);
        primFace++;
//        SetUpMaterial(mesh, primFace, baseMaterial, sop->textures[primFace]);
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
    
    mat->SetScalarParameterValue(TEXT("Emissive"), (te.material & 0x20) ? 1.0f : 0.0f);
    mat->SetVectorParameterValue(TEXT("Color"), te.color);
    mat->SetVectorParameterValue(TEXT("Offset"), FLinearColor(te.offsetU, -te.offsetV, 0.0f, 0.0f));
    mat->SetVectorParameterValue(TEXT("Repeats"), FLinearColor(te.repeatU, te.repeatV, 0.0f, 0.0f));
    mat->SetScalarParameterValue(TEXT("Rotation"), te.rotation);
    

    return mat;
}

/*
void AMeshActor::GotTexture(FGuid id, int status, UTexture2D *texture, UProceduralMeshComponent *mesh, int textureIndex, SceneObjectPart *sop)
{
    TextureEntry te = sop->textures[textureIndex];
    UMaterialInstanceDynamic* mat;

    if (!texture)
    {
        mat = SetUpMaterial(mesh, textureIndex, baseMaterial, te);
        
        return;
    }
    
    if (objectTextures.Contains(id))
    {
        UTexture2D *t = objectTextures[id];
        if (!t->CompressionNoAlpha)
            mat = SetUpMaterial(mesh, textureIndex, baseMaterialTranslucent, te);
        else
            mat = SetUpMaterial(mesh, textureIndex, baseMaterial, te);
        
        mat->SetTextureParameterValue(TEXT("Texture"), t);
    }
    else
    {
        UTexture2D *tex = DuplicateObject(texture, this);
        tex->CompressionNoAlpha = texture->CompressionNoAlpha;
        
        objectTextures.Add(id, tex);
        if (!tex->CompressionNoAlpha)
            mat = SetUpMaterial(mesh, textureIndex, baseMaterialTranslucent, te);
        else
            mat = SetUpMaterial(mesh, textureIndex, baseMaterial, te);
        mat->SetTextureParameterValue(TEXT("Texture"), tex);
    }
}
*/