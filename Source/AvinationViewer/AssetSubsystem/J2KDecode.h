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
    
    bool Decode(TSharedPtr<TArray<uint8_t>, ESPMode::ThreadSafe> data);
    opj_image *image = 0;

private:
    
};
