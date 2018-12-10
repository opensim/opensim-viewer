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
#pragma once

#include "AssetBase.h"
#include "AssetContainers.h"
#include "Http.h"


/**
 * 
 */

class AssetCache;

class AVINATIONVIEWER_API AssetDecodeThread : public FRunnable
{
public:
    AssetDecodeThread(AssetCache *c) { cache = c; thread = FRunnableThread::Create(this, TEXT("AssetDecode"), 0, TPri_BelowNormal); }
    virtual ~AssetDecodeThread() { Stop(); thread->WaitForCompletion(); delete thread; }
    
    virtual bool Init() override { return true; }
    uint32_t Run();
    virtual void Stop() override { runThis = false; }
    
private:
    FRunnableThread *thread;
    bool runThis = true;
    AssetCache *cache;
};

class AVINATIONVIEWER_API AssetProcessThread : public FRunnable
{
public:
    AssetProcessThread(AssetCache *c) { cache = c; thread = FRunnableThread::Create(this, TEXT("AssetDecode"), 0, TPri_BelowNormal); }
    virtual ~AssetProcessThread() { Stop(); thread->WaitForCompletion(); delete thread; }
    
    virtual bool Init() override { return true; }
    uint32_t Run();
    virtual void Stop() override { runThis = false; }
    
private:
    void DoCacheExpire();

    FRunnableThread *thread;
    bool runThis = true;
    AssetCache *cache;
    int cacheDivider;
};

class AVINATIONVIEWER_API AssetCache
{
public:
    static AssetCache& Get();
    static AssetCache& GetTexCache();
	virtual ~AssetCache();
    
    template <typename T>
    void Fetch(FGuid id, AssetFetchedDelegate delegate);
    
    void Tick();
    
protected:
    AssetCache();
    
    bool inline AddCallbackIfEnqueued(FGuid id, AssetFetchedDelegate d);
    void inline Enqueue(TSharedAssetFetchContainerRef fetch, int queue);
    bool inline MoveToQueue(TSharedAssetFetchContainerRef fetch, int queue);
    void RequestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful, TSharedAssetFetchContainerRef container);
    FString inline CreateURL(FGuid id);
    bool StartRequestFromWaitQueue();
    void StartRequests();
    
    TMap<FGuid, TSharedAssetFetchContainerRef> memCache;
    TMap<FGuid, TSharedAssetFetchContainerRef> queueIndex;
    
    TArray<TSharedAssetFetchContainerRef> queues[7];
    
    FCriticalSection queueLock;
    
    static AssetCache *instance;
    static AssetCache *textureInstance;
    
    AssetDecodeThread *decodeThread;
    AssetProcessThread *processThread;
    
    friend class AssetProcessThread;
    friend class AssetDecodeThread;
};
