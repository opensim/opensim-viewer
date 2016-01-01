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
#include <unistd.h>
#include <stdio.h>

// Sets default values
AMeshActor::AMeshActor()
{
    PrimaryActorTick.bCanEverTick = false;
 
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

    UProceduralMeshComponent *mesh = BuildComponent(sog->GetRootPart());
    
    SetRootComponent(mesh);

    mesh->SetRelativeLocation(FVector(0.0f));
    mesh->SetWorldScale3D(sog->GetRootPart()->scale);
    
    int index = 0;
    
    TArray<SceneObjectBase *> parts = sog->GetParts();
    
    for (auto it = parts.CreateConstIterator() ; it ; ++it)
    {
        SceneObjectPart *sop = (SceneObjectPart *)(*it);
        
        // Skip root part
        if (index > 0)
        {
            UProceduralMeshComponent *subMesh = BuildComponent(sop);

            //subMesh->AttachParent = mesh;
            subMesh->AttachTo(RootComponent);
            subMesh->SetWorldScale3D(sop->scale);
            FVector p = sop->position * 100;
            subMesh->SetRelativeLocation(p / sog->GetRootPart()->scale);
            subMesh->SetRelativeRotation(sop->rotation);
        }
        ++index;
    }
    
    OnObjectReady.ExecuteIfBound(this);
}

void AMeshActor::RegsterComponents()
{
    for (auto it = GetComponents().CreateConstIterator() ; it ; ++it)
    {
        (*it)->RegisterComponent();
    }
}

UProceduralMeshComponent *AMeshActor::BuildComponent(SceneObjectPart *sop)
{
    if (sop->sculptType == 0) // Prim
        return BuildComponentFromPrim(sop);
    else if (sop->sculptType == 5) // Mesh
        return BuildComponentFromMesh(sop);
    else
        return BuildComponentFromSculpt(sop);
    
    return 0;
}

UProceduralMeshComponent *AMeshActor::BuildComponentFromMesh(SceneObjectPart *sop)
{
    UProceduralMeshComponent *mesh = NewObject<UProceduralMeshComponent>(this, *sop->uuid.ToString());
    
    mesh->Mobility = EComponentMobility::Movable;
    //mesh->RegisterComponent();
    
    
    LLSDItem * data = sop->GetMeshData();
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
                
                minX = (float)(min->arrayData[0]->data.doubleData) * 100;
                minY = (float)(min->arrayData[1]->data.doubleData) * 100;
                minZ = (float)(min->arrayData[2]->data.doubleData) * 100;
                
                maxX = (float)(max->arrayData[0]->data.doubleData) * 100;
                maxY = (float)(max->arrayData[1]->data.doubleData) * 100;
                maxZ = (float)(max->arrayData[2]->data.doubleData) * 100;
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
            int normalsLength = (*face)->mapData[TEXT("Normal")]->binaryLength;
            int numVertices = binaryLength / 6; // 3 x uint16_t
            int numNormals = binaryLength / 6; // 3 x uint16_t
            
            uint16_t *vertexData = (uint16_t *)((*face)->mapData[TEXT("Position")]->data.binaryData);
            uint16_t *normalsData = (uint16_t *)((*face)->mapData[TEXT("Normal")]->data.binaryData);
            
            //UE_LOG(LogTemp, Warning, TEXT("Vertex count %d Normals count %d"), numVertices, numNormals);
            
            for (int idx = 0 ; idx < numVertices ; ++idx)
            {
                uint16_t posX = *vertexData++;
                uint16_t posY = *vertexData++;
                uint16_t posZ = *vertexData++;
                
                uint16_t norX = *normalsData++;
                uint16_t norY = *normalsData++;
                uint16_t norZ = *normalsData++;
                
                FVector pos(AvinationUtils::uint16tofloat(posX, minX, maxX),
                            -AvinationUtils::uint16tofloat(posY, minY, maxY),
                            AvinationUtils::uint16tofloat(posZ, minZ, maxZ));
                vertices.Add(pos);
                
                //UE_LOG(LogTemp, Warning, TEXT("Vertex %s"), *pos.ToString());
                
                if (idx < numNormals)
                {
                    FVector nor(AvinationUtils::uint16tofloat(norX, minX, maxX),
                                -AvinationUtils::uint16tofloat(norY, minY, maxY),
                                AvinationUtils::uint16tofloat(norZ, minZ, maxZ));
                    normals.Add(nor);
                }
                
                tangents.Add(FProcMeshTangent(1, 1, 1));
                vertexColors.Add(FColor(255, 255, 255, 255));
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
                                  -AvinationUtils::uint16tofloat(v, minV, maxV)));
            }
        }
        
        mesh->CreateMeshSection(textureIndex, vertices, triangles, normals, uv0, vertexColors, tangents, false);
        
        UMaterialInstanceDynamic* mat;
        
        TextureEntry te = sop->textures[textureIndex];
        
        if (objectTextures.Contains(te.textureId))
        {
            UTexture2D *t = objectTextures[te.textureId];
            if (!t->CompressionNoAlpha)
                mat = SetUpMaterial(mesh, textureIndex, baseMaterialTranslucent, te);
            else
                mat = SetUpMaterial(mesh, textureIndex, baseMaterial, te);
            mat->SetTextureParameterValue(TEXT("Texture"), t);
        }
        else
        {
            OnTextureFetched del;
            del.BindUObject(this, &AMeshActor::GotTexture, mesh, textureIndex, sop);
            TextureCache::Get().Fetch(te.textureId, del);
        }
        
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
            FVector v1(prim->viewerFaces[face].v1.X * 100.0f,
                       -prim->viewerFaces[face].v1.Y * 100.0f,
                       prim->viewerFaces[face].v1.Z * 100.0f);
            
            int i1 = vertices.Num();
            vertices.Add(v1);
            uv0.Add(FVector2D(prim->viewerFaces[face].uv1.U,
                              prim->viewerFaces[face].uv1.V));
            FVector n1(prim->viewerFaces[face].n1.X,
                      -prim->viewerFaces[face].n1.Y,
                      prim->viewerFaces[face].n1.Z);
            normals.Add(n1);
            tangents.Add(FProcMeshTangent(1, 1, 1));
            vertexColors.Add(FColor(255, 255, 255, 255));
            
            FVector v2(prim->viewerFaces[face].v2.X * 100.0f,
                       -prim->viewerFaces[face].v2.Y * 100.0f,
                       prim->viewerFaces[face].v2.Z * 100.0f);
            
            int i2 = vertices.Num();
            vertices.Add(v2);
            uv0.Add(FVector2D(prim->viewerFaces[face].uv2.U,
                              prim->viewerFaces[face].uv2.V));
            FVector n2(prim->viewerFaces[face].n2.X,
                      -prim->viewerFaces[face].n2.Y,
                      prim->viewerFaces[face].n2.Z);
            normals.Add(n2);
            tangents.Add(FProcMeshTangent(1, 1, 1));
            vertexColors.Add(FColor(255, 255, 255, 255));
            
            FVector v3(prim->viewerFaces[face].v3.X * 100.0f,
                       -prim->viewerFaces[face].v3.Y * 100.0f,
                       prim->viewerFaces[face].v3.Z * 100.0f);
            
            int i3 = vertices.Num();
            vertices.Add(v3);
            uv0.Add(FVector2D(prim->viewerFaces[face].uv3.U,
                              prim->viewerFaces[face].uv3.V));
            FVector n3(prim->viewerFaces[face].n3.X,
                      -prim->viewerFaces[face].n3.Y,
                      prim->viewerFaces[face].n3.Z);
            normals.Add(n3);
            tangents.Add(FProcMeshTangent(1, 1, 1));
            vertexColors.Add(FColor(255, 255, 255, 255));
            
            triangles.Add(i1);
            triangles.Add(i2);
            triangles.Add(i3);
            
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
        
        UMaterialInstanceDynamic* mat;
        
        TextureEntry te = sop->textures[primFace];
        
        if (objectTextures.Contains(te.textureId))
        {
            UTexture2D *t = objectTextures[te.textureId];
            if (!t->CompressionNoAlpha)
                mat = SetUpMaterial(mesh, primFace, baseMaterialTranslucent, te);
            else
                mat = SetUpMaterial(mesh, primFace, baseMaterial, te);
            mat->SetTextureParameterValue(TEXT("Texture"), t);
        }
        else
        {
            OnTextureFetched del;
            del.BindUObject(this, &AMeshActor::GotTexture, mesh, primFace, sop);
            TextureCache::Get().Fetch(te.textureId, del);
        }
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
    
    int numFaces = 1; // Sculpts have only one face
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
        
        FVector v1(vf.v1.X * 100.0f,
                   -vf.v1.Y * 100.0f,
                   vf.v1.Z * 100.0f);
        
        int i1 = vertices.Num();
        vertices.Add(v1);
        uv0.Add(FVector2D(vf.uv1.U,
                          -vf.uv1.V));
        FVector n1(vf.n1.X,
                  -vf.n1.Y,
                  vf.n1.Z);
        normals.Add(n1);
        tangents.Add(FProcMeshTangent(1, 1, 1));
        vertexColors.Add(FColor(255, 255, 255, 255));
        
        FVector v2(vf.v2.X * 100.0f,
                   -vf.v2.Y * 100.0f,
                   vf.v2.Z * 100.0f);
        
        int i2 = vertices.Num();
        vertices.Add(v2);
        uv0.Add(FVector2D(vf.uv2.U,
                          -vf.uv2.V));
        FVector n2(vf.n2.X,
                  -vf.n2.Y,
                  vf.n2.Z);
        normals.Add(n2);
        tangents.Add(FProcMeshTangent(1, 1, 1));
        vertexColors.Add(FColor(255, 255, 255, 255));
        
        FVector v3(vf.v3.X * 100.0f,
                   -vf.v3.Y * 100.0f,
                   vf.v3.Z * 100.0f);
        
        int i3 = vertices.Num();
        vertices.Add(v3);
        uv0.Add(FVector2D(vf.uv3.U,
                          -vf.uv3.V));
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
    
    UMaterialInstanceDynamic* mat;
    
    TextureEntry te = sop->textures[0];
    
    if (objectTextures.Contains(te.textureId))
    {
        UTexture2D *t = objectTextures[te.textureId];
        if (!t->CompressionNoAlpha)
            mat = SetUpMaterial(mesh, 0, baseMaterialTranslucent, te);
        else
            mat = SetUpMaterial(mesh, 0, baseMaterial, te);
        mat->SetTextureParameterValue(TEXT("Texture"), t);
    }
    else
    {
        OnTextureFetched del;
        del.BindUObject(this, &AMeshActor::GotTexture, mesh, 0, sop);
        TextureCache::Get().Fetch(te.textureId, del);
    }
    
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