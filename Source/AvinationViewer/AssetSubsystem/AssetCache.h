// Fill out your copyright notice in the Description page of Project Settings.

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
