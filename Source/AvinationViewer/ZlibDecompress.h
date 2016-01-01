// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <inttypes.h>

/**
 * 
 */
class AVINATIONVIEWER_API ZlibDecompress
{
public:
	ZlibDecompress();
	~ZlibDecompress();
    
    static int inflate(const uint8_t *src, uint32_t srcLen, uint8_t **dst);
};

