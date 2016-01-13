// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "SceneObjectBase.h"
#include "SceneObjectPart.h"
//DO NOT CHANGE CASING! (LGA) - Linux systems are case sensitive and most UE includes are camel case starting with capital letters.
#include "Core.h"

class AMeshActor;

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
    
    FCriticalSection fetchLock;
    int activeFetches;
    bool startingFetch;
    
    void CheckAssetsDone();
    virtual void RequestTextures() override;
    virtual SceneObject inline Type() override { return ObjectGroup; }
    AMeshActor *actor;
    
private:
    FCriticalSection partsLock;
    TArray<SceneObjectBase *> parts;
};
