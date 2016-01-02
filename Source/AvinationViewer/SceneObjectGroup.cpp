// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "SceneObjectGroup.h"
#include "RapidXml.h"

SceneObjectGroup::SceneObjectGroup()
{
    activeFetches = 0;
    startingFetch = false;
}

SceneObjectGroup::~SceneObjectGroup()
{
    for (auto part = parts.CreateIterator() ; part ; ++part)
    {
        delete (*part);
    }
    
    parts.Empty();
}

bool SceneObjectGroup::Load(rapidxml::xml_node<> *data)
{
    rapidxml::xml_node<> *partNode = data->first_node();
    if (!partNode)
        return false;
    if (FString(partNode->name()) == TEXT("RootPart"))
        partNode = partNode->first_node();
    
    SceneObjectPart *r = new SceneObjectPart();
    
    if (!r->Load(partNode))
        return false;
    
    r->parent = this;
    r->group = this;
    
    parts.Add(r);
    
    rapidxml::xml_node<> *kids = data->first_node()->next_sibling()->first_node();
    
    if (!kids)
        return true;
    
    for ( ; kids ; kids = kids->next_sibling())
    {
        SceneObjectPart *p = new SceneObjectPart();
        
        if (!p->Load(kids))
            return false;
        
        p->parent = r;
        p->group = this;
        
        parts.Add(p);
    }
    
    return true;
}

SceneObjectPart *SceneObjectGroup::GetRootPart()
{
    return (SceneObjectPart *)parts[0];
}

TArray<SceneObjectBase *> SceneObjectGroup::GetParts()
{
    TArray<SceneObjectBase *> ret;
    partsLock.Lock();
    ret.Append(parts);
    partsLock.Unlock();
    
    return ret;
}

void SceneObjectGroup::FetchAssets()
{
    startingFetch = true;
    partsLock.Lock();
    for (auto it = parts.CreateConstIterator() ; it ; ++it)
        (*it)->FetchAssets();
    partsLock.Unlock();
    startingFetch = false;
    CheckAssetsDone();
}

void SceneObjectGroup::CheckAssetsDone()
{
    if (startingFetch)
        return;
    
    fetchLock.Lock();
    if (activeFetches == 0 && !haveAllAssets)
    {
        haveAllAssets = true;
        OnObjectReady.ExecuteIfBound();
    }
    fetchLock.Unlock();
}

void SceneObjectGroup::GatherTextures()
{
    partsLock.Lock();
    for (auto it = parts.CreateConstIterator() ; it ; ++it)
        (*it)->GatherTextures();
    partsLock.Unlock();
}

void SceneObjectGroup::AddTexture(FGuid id)
{
    groupTextures.Add(id);
}