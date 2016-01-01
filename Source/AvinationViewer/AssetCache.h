// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "HttpAssetFetcher.h"

DECLARE_DELEGATE_ThreeParams(OnAssetFetched, FGuid, int, TArray<uint8_t>)

/**
 * 
 */
class AVINATIONVIEWER_API AssetCache
{
public:
    static AssetCache& Get();
	virtual ~AssetCache();
    
    void Fetch(FGuid id, OnAssetFetched delegate);
    
    
protected:
    AssetCache();
    TMap<FGuid, HttpAssetFetcher *> pendingFetches;
    TMap<FGuid, HttpAssetFetcher *> activeFetches;
    TMap<FGuid, TArray<uint8_t>> cache;
    
    void RequestDone(HttpAssetFetcher *req, FGuid id, int status, TArray<uint8_t> data);
    
    FPThreadsCriticalSection queueLock;
    
    static AssetCache *instance;
};
