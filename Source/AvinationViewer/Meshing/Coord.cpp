// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "Coord.h"
#include <math.h>

UVCoord::UVCoord(float u, float v)
{
    U = u;
    V = v;
}

UVCoord::UVCoord()
{
    U = 0;
    V = 0;
}
UVCoord::~UVCoord()
{
    
}

UVCoord UVCoord::Flip()
{
    U = 1.0f - U;
    V = 1.0f - V;

    return *this;
}