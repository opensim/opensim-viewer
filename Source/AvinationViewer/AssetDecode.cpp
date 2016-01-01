// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "AssetDecode.h"
#include <exception>
#include "Base64.h"

asset_decode_exception::asset_decode_exception(const char *d)
{
    data = d;
}

const char *asset_decode_exception::what() const noexcept
{
    return "asset_decode_exception";
}

AssetDecode::AssetDecode(TArray<uint8_t> asset)
{
    input.Empty();
    input.Append(asset);
    input.Add(0);
    
    doc.parse<0>((char *)(input.GetData()));
    
    rootNode = doc.first_node("AssetBase");
    if (rootNode == 0)
        throw asset_decode_exception("AssetBase missing");
    
    dataNode = rootNode->first_node("Data");
    if (dataNode == 0)
        throw asset_decode_exception("Data missing");
}

AssetDecode::~AssetDecode()
{
    doc.clear();
}

TArray<uint8_t> AssetDecode::AsArray()
{
    TArray<uint8_t> ret;
    ret.Append((uint8_t *)dataNode->value(), dataNode->value_size());
    return ret;
}

FString AssetDecode::AsString()
{
    return FString(dataNode->value());
}

TArray<uint8_t> AssetDecode::AsBase64DecodeArray()
{
    TArray<uint8_t> ret;
    FBase64::Decode(AsString(), ret);
    return ret;
}

FString AssetDecode::AsBase64DecodeString()
{
    FString src = AsString();
    FString ret;
    FBase64::Decode(src, ret);
    return ret;
}
