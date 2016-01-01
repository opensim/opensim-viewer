// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "openjpeg.h"

/**
 * 
 */
class AVINATIONVIEWER_API J2KDecode
{
public:
    J2KDecode();
	~J2KDecode();
    
    bool Decode(TArray<uint8_t> data);
    UTexture2D *CreateTexture(UObject *outer, FGuid id);
    opj_image *image = 0;

private:
    
    static UTexture2D* CreateTexture2D(int32 SrcWidth, int32 SrcHeight, const int32_t *r, const int32_t *g, const int32_t *b, const int32_t *a, UObject* Outer, const FString& Name, const EObjectFlags &Flags);
};
