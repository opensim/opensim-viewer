// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
#include "AssetBase.h"
#include "HttpAssetFetcher.h"
#include "SharedPointer.h"

/**
 * 
 */
class AVINATIONVIEWER_API AssetFetchContainer
{
public:
	AssetFetchContainer(TSharedRef<AssetBase, ESPMode::ThreadSafe> a, TSharedRef<IHttpRequest> r);
	~AssetFetchContainer();
    
    TSharedRef<AssetBase, ESPMode::ThreadSafe> asset;
    
private:
    TSharedRef<IHttpRequest> req;
};
