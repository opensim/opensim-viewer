// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <inttypes.h>
#include "Http.h"

/**
 * 
 */

/*
class HttpAssetFetcher;
class J2KDecode;

DECLARE_DELEGATE_TwoParams(AssetFetchedDelegate, FGuid, TArray<uint8_t>)
DECLARE_DELEGATE_TwoParams(TextureFetchedDelegate, FGuid, UTexture2D *)

DECLARE_DELEGATE_FourParams(RequestDoneDelegate, HttpAssetFetcher *, FGuid, int, TArray<uint8_t>)

class AVINATIONVIEWER_API HttpAssetFetcher
{
public:
	HttpAssetFetcher(FGuid id);
	~HttpAssetFetcher();
    
    void StartFetch();
    
    TArray<AssetFetchedDelegate> OnAssetFetched;
    TArray<TextureFetchedDelegate> OnTextureFetched;
    RequestDoneDelegate OnRequestDone;
    TArray<uint8_t> data;
    FGuid assetID;
    int status = 200;
    J2KDecode *dec = 0;
    UTexture2D *tex;
    
private:
    void requestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful);
};
*/