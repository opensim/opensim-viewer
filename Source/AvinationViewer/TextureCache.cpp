// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "TextureCache.h"
#include "J2KDecode.h"
#include "AssetDecode.h"

TextureCache *TextureCache::instance = 0;
//UObject *TextureCache::outer = 0;

TextureCache::TextureCache()
{
    thread = FRunnableThread::Create(this, TEXT("TextureCache"), 0, TPri_BelowNormal);
}

TextureCache::~TextureCache()
{
    stopThis = true;
    thread->WaitForCompletion();
    instance = 0;
}

TextureCache& TextureCache::Get()
{
    if (instance)
        return *instance;
    
    instance = new TextureCache();
    
    return *instance;
}

/*
void TextureCache::Fetch(FGuid id, OnTextureFetched delegate)
{
    //return;
    cacheLock.Lock();
    
    if (cache.Contains(id))
    {
        UTexture2D *tex = cache[id];
        
        cacheLock.Unlock();
        
        if (tex == 0)
        {
            delegate.ExecuteIfBound(id, 404, 0);
        }
        else
        {
            delegate.ExecuteIfBound(id, 200, tex);
        }
        return;
    }
    
    cacheLock.Unlock();
    queueLock.Lock();
    
    if (activeFetches.Contains(id))
    {
        activeFetches[id]->OnTextureFetched.Add(delegate);
    }
    else if(pendingFetches.Contains(id))
    {
        pendingFetches[id]->OnTextureFetched.Add(delegate);
    }
    else if(doneFetches.Contains(id))
    {
        doneFetches[id]->OnTextureFetched.Add(delegate);
    }
    else if(decodedFetches.Contains(id))
    {
        decodedFetches[id]->OnTextureFetched.Add(delegate);
    }
    else
    {
        HttpAssetFetcher *req = new HttpAssetFetcher(id);
        req->OnTextureFetched.Add(delegate);
        req->OnRequestDone.BindRaw(this, &TextureCache::RequestDone);
        
        if (activeFetches.Num() >= concurrentFetches)
        {
            pendingFetches.Add(id, req);
        }
        else
        {
            activeFetches.Add(id, req);
            req->StartFetch();
        }
    }
    
    queueLock.Unlock();
}

void TextureCache::RequestDone(HttpAssetFetcher *req, FGuid id, int status, TArray<uint8_t> data)
{
    if (status == -1)
    {
        // TODO: Implement retry
        queueLock.Lock();
        activeFetches.Remove(id);
        queueLock.Unlock();
        delete req;
        return;
    }
    
    if (data.Num() == 0)
    {
        // TODO: Intelligent failure handling
        queueLock.Lock();
        activeFetches.Remove(id);
        queueLock.Unlock();
        delete req;
        return;
    }
    
    queueLock.Lock();
    
    if (activeFetches.Contains(id))
    {
        activeFetches.Remove(id);
        if (status != -1)
            doneFetches.Add(id, req);
    }
    
    if (activeFetches.Num() < concurrentFetches)
    {
        auto it = pendingFetches.CreateIterator();
        if (it)
        {
            activeFetches.Add(it.Key(), it.Value());
            it.Value()->StartFetch();
            pendingFetches.Remove(it.Key());
        }
    }

    queueLock.Unlock();
    
    //while (ThreadedProcessDoneRequests())
    //    ;
    
    //while (DispatchDecodedRequest())
    //    ;
}

bool TextureCache::ThreadedProcessDoneRequests()
{
    queueLock.Lock();
    
    if (doneFetches.Num() == 0)
    {
        queueLock.Unlock();
        return false;
    }
    
    auto it = doneFetches.CreateConstIterator();
    
    if (!it)
    {
        queueLock.Unlock();
        return false;
    }
    
    FGuid id = it.Key();
    HttpAssetFetcher *req = it.Value();
    
    queueLock.Unlock();
    
    AssetDecode dec(req->data);
    
    req->dec = new J2KDecode();
    
    if (!req->dec->Decode(dec.AsBase64DecodeArray()))
    {
        queueLock.Lock();
        doneFetches.Remove(id);
        queueLock.Unlock();
        
        // TODO: Broken texture handling
        delete req;
        return true;
    }
    
    doneFetches.Remove(id);
    decodedFetches.Add(id, req);
    queueLock.Unlock();
    
    cacheLock.Unlock();
    
    return true;
}

bool TextureCache::DispatchDecodedRequest()
{
    if (currentDispatch)
    {
        TextureFetchedDelegate d = currentDispatch->OnTextureFetched[currentIndex];
        d.ExecuteIfBound(currentDispatch->assetID, currentDispatch->status, currentDispatch->tex);
        ++currentIndex;
        if (currentIndex >= currentDispatch->OnTextureFetched.Num())
        {
            delete currentDispatch;
            currentDispatch = 0;
        }
        return true;
    }
    
    queueLock.Lock();
    
    if (decodedFetches.Num() == 0)
    {
        queueLock.Unlock();
        return false;
    }
    
    auto it = decodedFetches.CreateConstIterator();
    
    if (!it)
    {
        queueLock.Unlock();
        return false;
    }
    
    FGuid id = it.Key();
    HttpAssetFetcher *req = it.Value();
    
    decodedFetches.Remove(id);
    
    queueLock.Unlock();

    cacheLock.Lock();
    req->tex = req->dec->CreateTexture(outer, id);
    
    // TODO: Fix cache
    //cache.Add(id, req->tex);
    cacheLock.Unlock();
    
    currentDispatch = req;
    currentIndex = 0;
    
    return true;
}
*/
uint32_t TextureCache::Run()
{
    while (!stopThis)
    {
//        while (ThreadedProcessDoneRequests())
//            ;
        
        usleep(10000);
    }
    return 0;
}

bool TextureCache::Init()
{
    return true;
}

void TextureCache::Stop()
{
    stopThis = true;
}