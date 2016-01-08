// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "Coord.h"

class Coord;

/**
 * 
 */

class AVINATIONVIEWER_API PrimQuat
{
public:
    PrimQuat();
    PrimQuat(float x, float y, float z, float w);
    PrimQuat(Coord axis, float angle);
    ~PrimQuat();
    
    float Length();
    PrimQuat Normalize();
    
    PrimQuat operator *(const PrimQuat& q1);
    FString ToString();
    
    float X;
    float Y;
    float Z;
    float W;
};
