// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "PrimQuat.h"

class PrimQuat;

/**
 * 
 */
class AVINATIONVIEWER_API Coord
{
public:
    Coord();
    Coord(float x, float y, float z);
	~Coord();
    
    float Length();
    Coord Invert();
    Coord Normalize();
    FString ToString();
    
    static Coord Cross(Coord c1, Coord c2);
    
    Coord operator+ (const Coord& a);
    Coord operator* (const Coord& m);
    Coord operator* (const PrimQuat& q);
    Coord operator= (const Coord& c2);
    Coord operator+= (const Coord& a);
    Coord operator*= (const Coord& m);
    Coord operator*= (const PrimQuat& q);
    bool operator == (const Coord& c2);
    float X;
    float Y;
    float Z;
};

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