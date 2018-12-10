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
#include "AvinationViewer.h"
#include "SceneObjectGroup.h"
#include "../Utils/RapidXml.h"

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

void SceneObjectGroup::RequestTextures()
{
    partsLock.Lock();
    for (auto it = parts.CreateConstIterator() ; it ; ++it)
        (*it)->RequestTextures();
    partsLock.Unlock();
}
