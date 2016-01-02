// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class SceneObjectGroup;
/**
 * 
 */
DECLARE_DELEGATE(SceneObjectReadyDelegate)

class AVINATIONVIEWER_API SceneObjectBase
{
public:
	SceneObjectBase();
	virtual ~SceneObjectBase();
    
    virtual void FetchAssets();
    SceneObjectBase *parent;
    SceneObjectGroup *group;
    
    virtual bool inline IsObjectPart() { return false; }
protected:
    bool haveAllAssets;

private:
    virtual void AssetReceived(FGuid id, TArray<uint8_t> data);
};
