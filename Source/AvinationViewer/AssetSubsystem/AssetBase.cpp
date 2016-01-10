// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "AssetBase.h"
#include "AssetDecode.h"

AssetBase::AssetBase()
{
    decode.BindRaw(this, &AssetBase::Decode);
}

AssetBase::~AssetBase()
{
}

void AssetBase::Decode(TArray<uint8_t>& data)
{
    
}