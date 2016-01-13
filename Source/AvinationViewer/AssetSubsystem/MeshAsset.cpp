// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
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
}