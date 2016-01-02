// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <inttypes.h>

typedef enum _LLSDType
{
    UNDEF,
    LBOOLEAN,
    INTEGER,
    DOUBLE,
    UUID,
    BINARY,
    STRING,
    URI,
    DATE,
    ARRAY,
    MAP
} LLSDType;


/**
 * 
 */
class AVINATIONVIEWER_API LLSDItem
{
public:
	~LLSDItem();
    LLSDItem();
    
    LLSDType type;
    uint32_t binaryLength;
    union
    {
        bool booleanData;
        long integerData;
        double doubleData;
        char *stringData;
        uint8_t *binaryData;
    } data;
    
    TArray<LLSDItem *> arrayData;
    TMap<FString, LLSDItem *> mapData;
};