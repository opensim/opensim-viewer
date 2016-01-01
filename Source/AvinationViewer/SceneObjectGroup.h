// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SceneObjectBase.h"
#include "SceneObjectPart.h"

/**
 * 
 */
class AVINATIONVIEWER_API SceneObjectGroup : public SceneObjectBase
{
public:
	SceneObjectGroup();
	virtual ~SceneObjectGroup();
    
    bool Load(rapidxml::xml_node<> *data);
    
    TArray<SceneObjectBase *> GetParts();
    SceneObjectPart *GetRootPart();
    
    void FetchAssets() override;
    SceneObjectReadyDelegate OnObjectReady;
    
    FPThreadsCriticalSection fetchLock;
    int activeFetches;
    bool startingFetch;
    
    void CheckAssetsDone();
private:
    FPThreadsCriticalSection partsLock;
    TArray<SceneObjectBase *> parts;
};
