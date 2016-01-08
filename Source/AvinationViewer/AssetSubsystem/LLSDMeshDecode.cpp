// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "LLSDMeshDecode.h"
#include "../Utils/ZlibDecompress.h"
#include "LLSDDecode.h"

LLSDMeshDecode::LLSDMeshDecode()
{
}

LLSDMeshDecode::~LLSDMeshDecode()
{
}

LLSDItem *LLSDMeshDecode::Decode(uint8_t *data, FString lod)
{
    LLSDDecode *dec = new LLSDDecode();
    
    uint8_t *d = data;
    
    dec->Decode(&d);
    
    TMap<FString, LLSDItem *> high_lod = dec->items->mapData[lod]->mapData;
    
    uint32_t offset = high_lod[TEXT("offset")]->data.integerData;
    uint32_t size = high_lod[TEXT("size")]->data.integerData;
    
    //    UE_LOG(LogTemp, Warning, TEXT("High LOD data at %lu, length %lu"), offset, size);
    
    uint32_t header_length = d - data;
    
    delete dec;
    
    uint8_t *outbuf = 0;
    
    int ret = ZlibDecompress::inflate(d + offset, size, &outbuf);
    
    //    UE_LOG(LogTemp, Warning, TEXT("ret = %d"), ret);
    
    if (ret <= 0)
        return 0;
    
    d = outbuf;
    
    dec = new LLSDDecode();
    
    dec->Decode(&d);
    
    LLSDItem *retval = dec->items;
    
    dec->items = 0;
    
    delete dec;
    
    return retval;
}