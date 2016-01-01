// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LLSDItem.h"

/**
 * 
 */
class AVINATIONVIEWER_API LLSDMeshDecode
{
public:
	LLSDMeshDecode();
	~LLSDMeshDecode();
    
    static LLSDItem *Decode(uint8_t *mesh, FString lod);
};
