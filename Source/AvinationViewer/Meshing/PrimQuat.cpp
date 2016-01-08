// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "PrimQuat.h"
#include <math.h>

PrimQuat::PrimQuat()
{
    X = Y = Z = 0.0f;
}

PrimQuat::PrimQuat(float x, float y, float z, float w)
{
    X = x;
    Y = y;
    Z = z;
    W = w;
}

PrimQuat::PrimQuat(Coord axis, float angle)
{
    axis = axis.Normalize();
    
    angle *= 0.5f;
    float c = (float)cos(angle);
    float s = (float)sin(angle);
    
    X = axis.X * s;
    Y = axis.Y * s;
    Z = axis.Z * s;
    W = c;
    
    Normalize();
}

PrimQuat::~PrimQuat()
{
}

float PrimQuat::Length()
{
    return (float)sqrt(X * X + Y * Y + Z * Z + W * W);
}

PrimQuat PrimQuat::Normalize()
{
    const float MAG_THRESHOLD = 0.0000001f;
    float mag = Length();
    
    // Catch very small rounding errors when normalizing
    if (mag > MAG_THRESHOLD)
    {
        float oomag = 1.0f / mag;
        X *= oomag;
        Y *= oomag;
        Z *= oomag;
        W *= oomag;
    }
    else
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
        W = 1.0f;
    }
    
    return *this;
}

PrimQuat PrimQuat::operator *(const PrimQuat& q2)
{
    float x = W * q2.X + X * q2.W + Y * q2.Z - Z * q2.Y;
    float y = W * q2.Y - X * q2.Z + Y * q2.W + Z * q2.X;
    float z = W * q2.Z + X * q2.Y - Y * q2.X + Z * q2.W;
    float w = W * q2.W - X * q2.X - Y * q2.Y - Z * q2.Z;
    
    return PrimQuat(x, y, z, w);
}

FString PrimQuat::ToString()
{
    return FString::Printf(TEXT("< X: %f, Y: %f, Z: %f, W: %f >"), X, Y, Z, W);
}