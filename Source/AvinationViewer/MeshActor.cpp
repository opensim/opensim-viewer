// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "MeshActor.h"
#include "LLSDItem.h"
#include "LLSDDecode.h"
#include "AvinationUtils.h"
#include "AssetCache.h"
#include "AssetDecode.h"
#include "TextureEntry.h"
#include "TextureCache.h"
#include "PrimMesher.h"

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

    mesh->AttachTo(RootComponent);
    // root part relative position and rotation should always be null
    // unless explicitly changed (as in edit parts on root prim)
    mesh->SetRelativeLocation(FVector(0.0f));
    mesh->SetWorldScale3D(sog->GetRootPart()->scale);
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
    
    if (sop->sculptType == 0) // Prim
        mesh = BuildComponentFromPrim(sop);
    else if (sop->sculptType == 5) // Mesh
        mesh = BuildComponentFromMesh(sop);
    else
        mesh = BuildComponentFromSculpt(sop);
    
    mesh->SetCullDistance(12800);
    mesh->SetCachedMaxDrawDistance(12800);
    
    return mesh;
}

UProceduralMeshComponent *AMeshActor::BuildComponentFromMesh(SceneObjectPart *sop)
{
    UProceduralMeshComponent *mesh = NewObject<UProceduralMeshComponent>(this, *sop->uuid.ToString());
    
    mesh->Mobility = EComponentMobility::Movable;
    
    //mesh->RegisterComponent();
    
    LLSDItem * data = sop->GetMeshData(1);
    if (!data)
        return mesh;
    
    int textureIndex = 0;
    
    for (auto face = data->arrayData.CreateConstIterator() ; face ; ++face)
    {
        float minX = -0.5f, minY = -0.5f, minZ = -0.5f, maxX = 0.5f, maxY = 0.5f, maxZ = 0.5f;
        float minU = 0.0f, minV = 0.0f, maxU = 0.0f, maxV = 0.0f;
        
        TArray<FVector> vertices;
        TArray<int32> triangles;
        TArray<FVector> normals;
        TArray<FVector2D> uv0;
        TArray<FColor> vertexColors;
        TArray<FProcMeshTangent> tangents;
        
        if (!(*face)->mapData.Contains(TEXT("NoGeometry")) || !(*face)->mapData[TEXT("NoGeometry")]->data.booleanData)
        {
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
            
            for (int idx = 0 ; idx < numVertices ; ++idx)
            {
                uint16_t posX = *vertexData++;
                uint16_t posY = *vertexData++;
                uint16_t posZ = *vertexData++;
                
                FVector pos(AvinationUtils::uint16tofloat(posX, minX, maxX),
                            -AvinationUtils::uint16tofloat(posY, minY, maxY),
                            AvinationUtils::uint16tofloat(posZ, minZ, maxZ));
                vertices.Add(pos);
                
                //UE_LOG(LogTemp, Warning, TEXT("Vertex %s"), *pos.ToString());
                
                tangents.Add(FProcMeshTangent(1, 1, 1));
                vertexColors.Add(FColor(255, 255, 255, 255));
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
                    normals.Add(nor);
                }
            }
            
            uint16_t numTriangles = (*face)->mapData[TEXT("TriangleList")]->binaryLength / 6; // 3 * uint16_t
            uint16_t *trianglesData = (uint16_t *)((*face)->mapData[TEXT("TriangleList")]->data.binaryData);
            
            //UE_LOG(LogTemp, Warning, TEXT("Triangles count %d"), numTriangles);
            for (int idx = 0 ; idx < numTriangles ; ++idx)
            {
                uint16_t t1 = *trianglesData++;
                uint16_t t2 = *trianglesData++;
                uint16_t t3 = *trianglesData++;
                triangles.Add(t1);
                triangles.Add(t2);
                triangles.Add(t3);
            }
            
            uint16_t numUvs = (*face)->mapData[TEXT("TexCoord0")]->binaryLength / 4; // 3 * uint16_t
            uint16_t *uvData = (uint16_t *)((*face)->mapData[TEXT("TexCoord0")]->data.binaryData);
            
            //UE_LOG(LogTemp, Warning, TEXT("UV count %d"), numUvs);
            
            for (int idx = 0 ; idx < numUvs ; ++idx)
            {
                uint16_t u = *uvData++;
                uint16_t v = *uvData++;
                
                uv0.Add(FVector2D(AvinationUtils::uint16tofloat(u, minU, maxU),
                                  1.0 - AvinationUtils::uint16tofloat(v, minV, maxV)));
            }
        }
        
        mesh->CreateMeshSection(textureIndex, vertices, triangles, normals, uv0, vertexColors, tangents, false);
        
        textureIndex++;
    }
    delete data;
    
    return mesh;
}

UProceduralMeshComponent *AMeshActor::BuildComponentFromPrim(SceneObjectPart *sop)
{
    UProceduralMeshComponent *mesh = NewObject<UProceduralMeshComponent>(this, *sop->uuid.ToString());
    
    mesh->Mobility = EComponentMobility::Movable;
    //mesh->RegisterComponent();
    mesh->bAutoRegister = false;
    
    if (!sop->MeshPrim())
        return mesh;
    
    int numFaces = sop->numFaces;
    PrimMesh *prim = sop->primData;
    
    int face = 0;

    for (int primFace = 0 ; primFace < numFaces ; primFace++)
    {
        TArray<FVector> vertices;
        TArray<int32> triangles;
        TArray<FVector> normals;
        TArray<FVector2D> uv0;
        TArray<FColor> vertexColors;
        TArray<FProcMeshTangent> tangents;
        
        while (face < prim->viewerFaces.Num() && prim->viewerFaces[face].primFaceNumber == primFace)
        {
            FVector v1(prim->viewerFaces[face].v1.X,
                       -prim->viewerFaces[face].v1.Y,
                       prim->viewerFaces[face].v1.Z);
            
            int i1 = vertices.Num();
            vertices.Add(v1);
            uv0.Add(FVector2D(prim->viewerFaces[face].uv1.U,
                               1.0f-prim->viewerFaces[face].uv1.V));
//                                prim->viewerFaces[face].uv1.V));
            FVector n1(prim->viewerFaces[face].n1.X,
                      -prim->viewerFaces[face].n1.Y,
                      prim->viewerFaces[face].n1.Z);
            normals.Add(n1);
            tangents.Add(FProcMeshTangent(1, 1, 1));
            vertexColors.Add(FColor(255, 255, 255, 255));
            
            FVector v2(prim->viewerFaces[face].v2.X,
                       -prim->viewerFaces[face].v2.Y,
                       prim->viewerFaces[face].v2.Z);
            
            int i2 = vertices.Num();
            vertices.Add(v2);
            uv0.Add(FVector2D(prim->viewerFaces[face].uv2.U,
                            1.0f - prim->viewerFaces[face].uv2.V));
//                            prim->viewerFaces[face].uv2.V));
            FVector n2(prim->viewerFaces[face].n2.X,
                      -prim->viewerFaces[face].n2.Y,
                      prim->viewerFaces[face].n2.Z);
            normals.Add(n2);
            tangents.Add(FProcMeshTangent(1, 1, 1));
            vertexColors.Add(FColor(255, 255, 255, 255));
            
            FVector v3(prim->viewerFaces[face].v3.X,
                       -prim->viewerFaces[face].v3.Y,
                       prim->viewerFaces[face].v3.Z);
            
            int i3 = vertices.Num();
            vertices.Add(v3);
            uv0.Add(FVector2D(prim->viewerFaces[face].uv3.U,
                            1.0f - prim->viewerFaces[face].uv3.V));
//                            prim->viewerFaces[face].uv3.V));
            FVector n3(prim->viewerFaces[face].n3.X,
                      -prim->viewerFaces[face].n3.Y,
                      prim->viewerFaces[face].n3.Z);
            normals.Add(n3);
            tangents.Add(FProcMeshTangent(1, 1, 1));
            vertexColors.Add(FColor(255, 255, 255, 255));
            
            triangles.Add(i1);
            triangles.Add(i2);
            triangles.Add(i3);
//            triangles.Add(i3);
//            triangles.Add(i2);

            /*
            uv0.Add(FVector2D(prim->viewerFaces[face].uv1.U,
                              prim->viewerFaces[face].uv1.V));
            uv0.Add(FVector2D(prim->viewerFaces[face].uv2.U,
                              prim->viewerFaces[face].uv2.V));
            uv0.Add(FVector2D(prim->viewerFaces[face].uv3.U,
                              prim->viewerFaces[face].uv3.V));
            */
            
            ++face;
        }
        
        mesh->CreateMeshSection(primFace, vertices, triangles, normals, uv0, vertexColors, tangents, false);
    }
    
    delete sop->primData;
    sop->primData = 0;
   
    
    return mesh;
}

UProceduralMeshComponent *AMeshActor::BuildComponentFromSculpt(SceneObjectPart *sop)
{
    /*
    int fd = open("/Users/melanie/tmp/mesh.obj", O_CREAT | O_TRUNC | O_WRONLY, 0644);
    
    FILE *fp = fdopen(fd, "w");
    */
    
    UProceduralMeshComponent *mesh = NewObject<UProceduralMeshComponent>(this, *sop->uuid.ToString());
    
    mesh->Mobility = EComponentMobility::Movable;
    //mesh->RegisterComponent();
    
    if (!sop->MeshPrim())
        return mesh;
    
    if (!sop->sculptData)
        return mesh;
    
    SculptMesh *prim = sop->sculptData;
    
    TArray<FVector> vertices;
    TArray<int32> triangles;
    TArray<FVector> normals;
    TArray<FVector2D> uv0;
    TArray<FColor> vertexColors;
    TArray<FProcMeshTangent> tangents;
    
    for (int face = 0 ; face < prim->viewerFaces.Num() ; ++face)
    {
        ViewerFace vf = prim->viewerFaces[face];
        
        FVector v1(vf.v1.X,
                   -vf.v1.Y,
                   vf.v1.Z);
        
        int i1 = vertices.Num();
        vertices.Add(v1);
        uv0.Add(FVector2D(vf.uv1.U,
                          1.0 - vf.uv1.V));
        FVector n1(vf.n1.X,
                  -vf.n1.Y,
                  vf.n1.Z);
        normals.Add(n1);
        tangents.Add(FProcMeshTangent(1, 1, 1));
        vertexColors.Add(FColor(255, 255, 255, 255));
        
        FVector v2(vf.v2.X,
                   -vf.v2.Y,
                   vf.v2.Z);
        
        int i2 = vertices.Num();
        vertices.Add(v2);
        uv0.Add(FVector2D(vf.uv2.U,
                          1.0 - vf.uv2.V));
        FVector n2(vf.n2.X,
                  -vf.n2.Y,
                  vf.n2.Z);
        normals.Add(n2);
        tangents.Add(FProcMeshTangent(1, 1, 1));
        vertexColors.Add(FColor(255, 255, 255, 255));
        
        FVector v3(vf.v3.X,
                   -vf.v3.Y,
                   vf.v3.Z);
        
        int i3 = vertices.Num();
        vertices.Add(v3);
        uv0.Add(FVector2D(vf.uv3.U,
                          1.0 - vf.uv3.V));
        FVector n3(vf.n3.X,
                  -vf.n3.Y,
                  vf.n3.Z);
        normals.Add(n3);
        tangents.Add(FProcMeshTangent(1, 1, 1));
        vertexColors.Add(FColor(255, 255, 255, 255));
        
        triangles.Add(i1);
        triangles.Add(i3);
        triangles.Add(i2);
    }
    
    /*
    for (auto it = vertices.CreateConstIterator() ; it ; ++it)
        fprintf(fp, "v %f %f %f\n", (*it).X, -(*it).Y, (*it).Z);
    
    for (auto it = normals.CreateConstIterator() ; it ; ++it)
        fprintf(fp, "vn %f %f %f\n", (*it).X, (*it).Y, (*it).Z);
    
    for (auto it = uv0.CreateConstIterator() ; it ; ++it)
        fprintf(fp, "vt %f %f %f\n", (*it).X, -(*it).Y, 0.0f);
    
    for (int i = 0 ; i < triangles.Num() ; i += 3)
        fprintf(fp, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                triangles[i] + 1, triangles[i] + 1, triangles[i] + 1,
                triangles[i + 1] + 1, triangles[i + 1] + 1, triangles[i + 1] + 1,
                triangles[i + 2] + 1, triangles[i + 2] + 1, triangles[i + 2] + 1);
    
    fclose(fp);
    close(fd);
    */
    mesh->CreateMeshSection(0, vertices, triangles, normals, uv0, vertexColors, tangents, false);
    
    delete sop->sculptData;
    sop->sculptData = 0;
    
    return mesh;
}

UMaterialInstanceDynamic *AMeshActor::SetUpMaterial(UProceduralMeshComponent *mesh, int textureIndex, UMaterial *baseMat, TextureEntry& te)
{
    //if (baseMat == baseMaterialTranslucent)
    //    UE_LOG(LogTemp, Warning, TEXT("Translucent texture"));
    
    mesh->SetMaterial(textureIndex, baseMat);
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