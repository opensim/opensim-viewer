// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "SceneObjectBase.h"
#include "SceneObjectGroup.h"

SceneObjectBase::SceneObjectBase()
{
    haveAllAssets = false;
}

SceneObjectBase::~SceneObjectBase()
{
}

void SceneObjectBase::AssetReceived(FGuid id, TArray<uint8_t> data)
{
    
}

void SceneObjectBase::FetchAssets()
{
    
}