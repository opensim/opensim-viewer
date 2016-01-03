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

void TextureCache::Fetch(FGuid id, OnTextureFetched delegate)
{
    HttpAssetFetcher *req = new HttpAssetFetcher(id);
    
    req->OnTextureFetched.Add(delegate);
    req->OnRequestDone.BindRaw(this, &TextureCache::RequestDone);
    
    queueLock.Lock();
    if (activeFetches.Num() >= concurrentFetches)
    {
        pendingFetches.Add(id, req);
        queueLock.Unlock();
    }
    else
    {
        activeFetches.Add(id, req);
        queueLock.Unlock();
        req->StartFetch();
    }
    
}

void TextureCache::RequestDone(HttpAssetFetcher *req, FGuid id, int status, TArray<uint8_t> data)
{
    queueLock.Lock();
    if (!activeFetches.Contains(id))
    {
        queueLock.Unlock();
        delete req;
        return;
    }
    
    while (activeFetches.Num() - 1 < concurrentFetches)
    {
        auto it = pendingFetches.CreateIterator();
        if (it)
        {
            activeFetches.Add(it.Key(), it.Value());
            it.Value()->StartFetch();
            pendingFetches.Remove(it.Key());
        }
    }
    
    if (status == -1)
    {
        // TODO: Implement retry
        activeFetches.Remove(id);
        queueLock.Unlock();
        delete req;
        return;
    }
    
    if (data.Num() == 0)
    {
        // TODO: Intelligent failure handling
        activeFetches.Remove(id);
        queueLock.Unlock();
        delete req;
        return;
    }
    
    activeFetches.Remove(id);
    doneFetches.Add(id, req);
    
    queueLock.Unlock();
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

*/
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
    
    if (req->status < 0)
        return false;
    
    J2KDecode jdec;
    
    try
    {
        AssetDecode dec(req->data);
        if (!jdec.Decode(dec.AsBase64DecodeArray()))
        {
            delete req;
            return true;
        }
    }
    catch (std::exception& ex)
    {
        delete req;
        return true;
    }
    
    UTexture2D *tex = UTexture2D::CreateTransient(jdec.image->comps[0].w, jdec.image->comps[0].h);
    
    for (auto it2 = req->OnTextureFetched.CreateConstIterator() ; it2 ; ++it2)
    {
        (*it2).ExecuteIfBound(id, 0);
    }
    
    queueLock.Lock();
    doneFetches.Remove(id);
//    decodedFetches.Add(id, req);
    queueLock.Unlock();
    
    delete req;
    
    return true;
}

/*
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
        while (ThreadedProcessDoneRequests())
            ;
        
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