// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "AssetBase.h"
#include "AssetDecode.h"

AssetBase::AssetBase()
{
    state = AssetState::Waiting;
    
    decode.BindRaw(this, &AssetBase::Decode);
}

AssetBase::~AssetBase()
{
}

void AssetBase::Decode()
{
    // The asset decoder WILL destroy the stageData. Rapidxml::parse modifies
    // it's input in place!
    AssetDecode dec(*stageData);
    stageData->Empty();
    stageData->Append(dec.AsBase64DecodeArray());
}