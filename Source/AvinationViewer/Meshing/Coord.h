// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

/**
 * 
 */

class AVINATIONVIEWER_API UVCoord
{
public:
    UVCoord();
    UVCoord(float u, float v);
    ~UVCoord();
    
    UVCoord Flip();
    
    float U;
    float V;
};