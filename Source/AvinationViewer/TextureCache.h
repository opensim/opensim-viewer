// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "ImageUtils.h"
#include "HttpAssetFetcher.h"

DECLARE_DELEGATE_TwoParams(OnTextureFetched, FGuid, UTexture2D *)

/**
 * 
 */
class AVINATIONVIEWER_API TextureCache : public FRunnable
{
public:
	~TextureCache();
    static TextureCache& Get();
    
    void Fetch(FGuid id, OnTextureFetched delegate);
    
    //static UObject *outer;
    
    //bool ThreadedProcessDoneRequests();
    //bool DispatchDecodedRequest();

    virtual bool Init() override;
    uint32_t Run();
    virtual void Stop() override;
    
protected:
    TextureCache();
    
    static TextureCache *instance;
    FRunnableThread *thread;
    
    bool stopThis = false;
    
    int concurrentFetches = 2;
    TMap<FGuid, HttpAssetFetcher *> pendingFetches;
    TMap<FGuid, HttpAssetFetcher *> activeFetches;
    FCriticalSection queueLock;
    
    void RequestDone(HttpAssetFetcher *req, FGuid id, int status, TArray<uint8_t> data);
    /*
    TMap<FGuid, HttpAssetFetcher *> doneFetches;
    TMap<FGuid, HttpAssetFetcher *> decodedFetches;
    TMap<FGuid, UTexture2D *> cache;
    
    
    FPThreadsCriticalSection cacheLock;
     HttpAssetFetcher *currentDispatch = 0;
     int currentIndex = 0;

    */
};
