// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "HttpAssetFetcher.h"
#include "J2KDecode.h"

HttpAssetFetcher::HttpAssetFetcher(FGuid id)
{
    assetID = id;
}

HttpAssetFetcher::~HttpAssetFetcher()
{
    if (dec)
        delete dec;
}

void HttpAssetFetcher::StartFetch()
{
    FString url(TEXT("http://grid-a.3dhosting.de:8400/assets/"));
    
    if (assetID.A > 0x7f000000)
    {
        url = TEXT("http://grid-b.3dhosting.de:8400/assets/");
    }
    
    url.Append(assetID.ToString(EGuidFormats::DigitsWithHyphens).ToLower());
    
    //UE_LOG(LogTemp, Warning, TEXT("URL is %s"), *url);
    
    TSharedRef<IHttpRequest> req = (&FHttpModule::Get())->CreateRequest();
    req->SetHeader(TEXT("Authorization"), TEXT("Basic YXNzZXRzOmdqMzI5dWQ="));
    req->SetVerb(TEXT("GET"));
    req->SetURL(url);
    req->OnProcessRequestComplete().BindRaw(this, &HttpAssetFetcher::requestDone);
    req->ProcessRequest();
}

void HttpAssetFetcher::requestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
{
    if (!response.IsValid())
    {
        OnRequestDone.ExecuteIfBound(this, assetID, -1, TArray<uint8_t>());
        return;
    }
    else if (EHttpResponseCodes::IsOk(response->GetResponseCode()))
    {
        status = response->GetResponseCode();
        
        // valid response
        data = response->GetContent();
        if (data.Num() == 0)
        {
            OnRequestDone.ExecuteIfBound(this, assetID, 404, data);
        }
        else
        {
            OnRequestDone.ExecuteIfBound(this, assetID, response->GetResponseCode(), data);
        }
    }
    else
    {
        OnRequestDone.ExecuteIfBound(this, assetID, response->GetResponseCode(),TArray<uint8_t>());
        return;
    }
}