// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "AssetContainers.h"

AssetFetchContainer::AssetFetchContainer(TSharedRef<AssetBase, ESPMode::ThreadSafe> a, TSharedRef<IHttpRequest> r) : asset(a), req(r)
{
}

AssetFetchContainer::~AssetFetchContainer()
{
}
