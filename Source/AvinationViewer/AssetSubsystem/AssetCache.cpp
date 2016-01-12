// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "AssetCache.h"
#include "AssetContainers.h"
#include "MeshAsset.h"
#include "SculptAsset.h"
#include "PrimAsset.h"

#define MAX_CONCURRENT_FETCHES 10

enum QueueNumber : int
{
    Wait = 0,
    Fetch = 1,
    Decode = 2,
    Preprocess = 3,
    MainProcess = 4,
    Dispatch = 5,
    Retry = 6
};

AssetCache *AssetCache::instance = 0;

AssetCache::AssetCache()
{
    decodeThread = new AssetDecodeThread(this);
    processThread = new AssetProcessThread(this);
}

AssetCache::~AssetCache()
{
    delete decodeThread;
    delete processThread;
    
    instance = 0;
}

AssetCache& AssetCache::Get()
{
    if (instance)
        return *instance;
    
    instance = new AssetCache();
    
    return *instance;
}

template<typename T>
void AssetCache::Fetch(FGuid id, AssetFetchedDelegate delegate)
{
    FScopeLock l(&queueLock);
    
    if (memCache.Contains(id))
    {
        Enqueue(memCache[id], QueueNumber::Dispatch);
        if (!AddCallbackIfEnqueued(id, delegate))
            return;
        StartRequests();
        
        return;
    }
    
    TSharedAssetFetchContainerRef container = MakeShareable(new AssetFetchContainer(id, MakeShareable(new T())));
    
    container->asset->state = AssetBase::Waiting;
    
    Enqueue(container, QueueNumber::Wait);
    
    if (!AddCallbackIfEnqueued(id, delegate))
        return;
    
    StartRequests();
}

void AssetCache::StartRequests()
{
    while (queues[QueueNumber::Fetch].Num() < MAX_CONCURRENT_FETCHES)
    {
        if (!StartRequestFromWaitQueue())
            return;
    }
}

bool AssetCache::StartRequestFromWaitQueue()
{
    if (queues[QueueNumber::Wait].Num() == 0)
        return false;
    
    TSharedAssetFetchContainerRef container = queues[QueueNumber::Wait][0];
    
    container->asset->state = AssetBase::Processing;
    TSharedRef<IHttpRequest> req = (&FHttpModule::Get())->CreateRequest();
    container->req = req;
    req->SetHeader(TEXT("Authorization"), TEXT("Basic YXNzZXRzOmdqMzI5dWQ="));
    req->SetVerb(TEXT("GET"));
    req->SetURL(CreateURL(container->id));
    req->OnProcessRequestComplete().BindRaw(this, &AssetCache::RequestDone, container);
    
    MoveToQueue(container, QueueNumber::Fetch);
    
    req->ProcessRequest();
    
    return true;
}

void AssetCache::RequestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful, TSharedAssetFetchContainerRef container)
{
    FScopeLock l(&queueLock);
    
    container->req.Reset();
    
    // Timeout, network error, et al.
    if (!response.IsValid())
    {
        container->asset->state = AssetBase::Retrying;
        MoveToQueue(container, QueueNumber::Retry);
    }
    else if (EHttpResponseCodes::IsOk(response->GetResponseCode()))
    {
        // Empty assets are fails
        if (response->GetContentLength() == 0)
        {
            container->asset->state = AssetBase::Failed;
            MoveToQueue(container, QueueNumber::Dispatch);
        }
        // Valid response
        TSharedPtr<TArray<uint8_t>, ESPMode::ThreadSafe> data(new TArray<uint8_t>());
        data->Append((const uint8_t *)response->GetContent().GetData(), response->GetContent().Num());
      
        container->asset->stageData = data;
        
        MoveToQueue(container, QueueNumber::Decode);
    }
    else
    {
        // Asset server gave an error
        container->asset->state = AssetBase::Failed;
        MoveToQueue(container, QueueNumber::Dispatch);
    }
    
    StartRequests();
}

bool inline AssetCache::AddCallbackIfEnqueued(FGuid id, AssetFetchedDelegate d)
{
    if (!queueIndex.Contains(id))
        return false;
    
    TSharedAssetFetchContainerRef fetch = queueIndex[id];
    
    fetch->AddDispatch(d);
    
    return true;
}

void inline AssetCache::Enqueue(TSharedAssetFetchContainerRef fetch, int queue)
{
    if (queueIndex.Contains(fetch->id))
        return;
    
    fetch->queue = queue;
    queues[queue].Add(fetch);
    queueIndex.Add(fetch->id, fetch);
}

bool inline AssetCache::MoveToQueue(TSharedAssetFetchContainerRef fetch, int queue)
{
    if (!queueIndex.Contains(fetch->id))
        return false;
    if (fetch->queue == queue)
        return true;
    queues[fetch->queue].Remove(fetch);
    fetch->queue = queue;
    queues[queue].Add(fetch);
    return true;
}

FString inline AssetCache::CreateURL(FGuid id)
{
    FString url(TEXT("http://grid-a.3dhosting.de:8400/assets/"));
    
    if (id.A > 0x7f000000)
    {
        url = TEXT("http://grid-b.3dhosting.de:8400/assets/");
    }
    
    url.Append(id.ToString(EGuidFormats::DigitsWithHyphens).ToLower());

    return url;
}

void AssetCache::Tick()
{
    queueLock.Lock();
    
    if (queues[QueueNumber::Preprocess].Num())
    {
        TSharedAssetFetchContainerRef fetch = queues[QueueNumber::Preprocess][0];
        
        queueLock.Unlock();
        
        try
        {
            fetch->asset->preProcess.ExecuteIfBound();
            queueLock.Lock();
            MoveToQueue(fetch, QueueNumber::MainProcess);
            queueLock.Unlock();
        }
        catch (...)
        {
            fetch->asset->SetFailed();
            queueLock.Lock();
            MoveToQueue(fetch, QueueNumber::Dispatch);
            queueLock.Unlock();
        }
    }
    else
    {
        queueLock.Unlock();
    }
    
    queueLock.Lock();
    
    if (queues[QueueNumber::Dispatch].Num())
    {
        TSharedAssetFetchContainerRef fetch = queues[QueueNumber::Dispatch][0];
        
        queueLock.Unlock();
        
        try
        {
            fetch->asset->postProcess.ExecuteIfBound();
        }
        catch (...)
        {
            fetch->asset->SetFailed();
        }
        
        fetch->asset->decode.Unbind();
        fetch->asset->preProcess.Unbind();
        fetch->asset->mainProcess.Unbind();
        fetch->asset->postProcess.Unbind();
        
        queueLock.Lock();
        queueIndex.Remove(fetch->id);
        queues[QueueNumber::Dispatch].RemoveAt(0);
        if (!memCache.Contains(fetch->id))
            memCache.Add(fetch->id, fetch);
        
        // We need to pull these here, under lock, because the request
        // may be requeued for dispatch from memory cache. To avoid
        // duplicating or losing dispatches, we make this local copy.
        TArray<AssetFetchedDelegate> dispatches = fetch->dispatches;
        fetch->dispatches.Empty();
        queueLock.Unlock();
        
        fetch->Dispatch(dispatches);
    }
    else
    {
        queueLock.Unlock();
    }
}

void TemplateInstantiator()
{
    AssetFetchedDelegate d;
    AssetCache::Get().Fetch<MeshAsset>(FGuid(), d);
    AssetCache::Get().Fetch<SculptAsset>(FGuid(), d);
    AssetCache::Get().Fetch<PrimAsset>(FGuid(), d);
}

uint32_t AssetDecodeThread::Run()
{
    while (runThis)
    {
        cache->queueLock.Lock();
        
        if (cache->queues[QueueNumber::Decode].Num())
        {
            TSharedAssetFetchContainerRef fetch = cache->queues[QueueNumber::Decode][0];
            
            cache->queueLock.Unlock();
            
            try
            {
                fetch->asset->decode.ExecuteIfBound();
                cache->queueLock.Lock();
                cache->MoveToQueue(fetch, QueueNumber::Preprocess);
                cache->queueLock.Unlock();
            }
            catch (...)
            {
                fetch->asset->SetFailed();
                cache->queueLock.Lock();
                cache->MoveToQueue(fetch, QueueNumber::Dispatch);
                cache->queueLock.Unlock();
            }
            
            usleep(1000);
        }
        else
        {
            cache->queueLock.Unlock();
            usleep(100000);
        }
    }
    return 0;
}

uint32_t AssetProcessThread::Run()
{
    while (runThis)
    {
        cache->queueLock.Lock();
        
        if (cache->queues[QueueNumber::MainProcess].Num())
        {
            TSharedAssetFetchContainerRef fetch = cache->queues[QueueNumber::MainProcess][0];
            
            cache->queueLock.Unlock();
            
            fetch->asset->mainProcess.ExecuteIfBound();
            
            cache->queueLock.Lock();
            cache->MoveToQueue(fetch, QueueNumber::Dispatch);
            cache->queueLock.Unlock();
            usleep(1000);
        }
        else
        {
            cache->queueLock.Unlock();
            usleep(100000);
        }
    }
    return 0;
}