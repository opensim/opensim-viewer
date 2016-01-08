// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "AssetCache.h"

AssetCache *AssetCache::instance = 0;

AssetCache::AssetCache()
{
}

AssetCache::~AssetCache()
{
    instance = 0;
}

AssetCache& AssetCache::Get()
{
    if (instance)
        return *instance;
    
    instance = new AssetCache();
    
    return *instance;
}

void AssetCache::Fetch(FGuid id, OnAssetFetched delegate)
{
    try
    {
        queueLock.Lock();
        
        if (cache.Contains(id))
        {
            TArray<uint8_t> data = cache[id];
            
            queueLock.Unlock();
            
            if (data.Num() == 0)
            {
                delegate.ExecuteIfBound(id, data);
            }
            else
            {
                delegate.ExecuteIfBound(id, data);
            }
            return;
        }
        if (activeFetches.Contains(id))
        {
            activeFetches[id]->OnAssetFetched.Add(delegate);
            queueLock.Unlock();
        }
        else
        {
            HttpAssetFetcher *req = new HttpAssetFetcher(id);
            req->OnAssetFetched.Add(delegate);
            req->OnRequestDone.BindRaw(this, &AssetCache::RequestDone);
            activeFetches.Add(id, req);
            queueLock.Unlock();
            req->StartFetch();
        }
    }
    catch(std::exception& e)
    {
        queueLock.Unlock();
        throw e;
    }
}

void AssetCache::RequestDone(HttpAssetFetcher *req, FGuid id, int status, TArray<uint8_t> data)
{
    queueLock.Lock();
    
    if (activeFetches.Contains(id))
    {
        activeFetches.Remove(id);
    }
    
    // TODO: Add a retry queue and timed retries instead of sending error reply
    if (status != -1) // -1 is retryable, so we don't want to cache a negative
        cache.Add(id, data);
    
    queueLock.Unlock();
    
    for (auto it = req->OnAssetFetched.CreateConstIterator() ; it ; ++it)
    {
        (*it).ExecuteIfBound(id, data);
    }
    
    delete req;
}