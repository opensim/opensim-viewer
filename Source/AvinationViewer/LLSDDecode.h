// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "LLSDItem.h"

/**
 * 
 */
class AVINATIONVIEWER_API LLSDDecode
{
public:
	LLSDDecode();
	~LLSDDecode();
    
    LLSDItem *items;
    
    bool Decode(uint8_t **data);
    void DebugDump();
    static void DumpItem(LLSDItem *item, bool indent = true);
    
private:
    LLSDItem *DecodeItem(uint8_t **data);
    double ntohd(double n);
};
