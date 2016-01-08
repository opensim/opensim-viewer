// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "Coord.h"
#include <math.h>

Coord::Coord(float x, float y, float z)
{
    X = x;
    Y = y;
    Z = z;
}

Coord::Coord()
{
    X = 0;
    Y = 0;
    Z = 0;
}

Coord::~Coord()
{
}

float Coord::Length()
{
    return (float)sqrt(X * X + Y * Y + Z * Z);
}

Coord Coord::Invert()
{
    X = -X;
    Y = -Y;
    Z = -Z;
    
    return *this;
}

Coord Coord::Normalize()
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
    }
    else
    {
        X = 0.0f;
        Y = 0.0f;
        Z = 0.0f;
    }
    
    return *this;
}

FString Coord::ToString()
{
    return FString::Printf(TEXT("%f %f %f"), X, Y, Z);
}

Coord Coord::Cross(Coord c1, Coord c2)
{
    return Coord(
                     c1.Y * c2.Z - c2.Y * c1.Z,
                     c1.Z * c2.X - c2.Z * c1.X,
                     c1.X * c2.Y - c2.X * c1.Y
                     );
}

Coord Coord::operator +(const Coord& a)
{
    return Coord(X + a.X, Y + a.Y, Z + a.Z);
}

Coord Coord::operator *(const Coord& m)
{
    return Coord(X * m.X, Y * m.Y, Z * m.Z);
}

Coord Coord::operator *(const PrimQuat& q)
{
    // From http://www.euclideanspace.com/maths/algebra/realNormedAlgebra/quaternions/transforms/
    
    Coord c2(0.0f, 0.0f, 0.0f);
    
    c2.X = q.W * q.W * X +
    2.0f * q.Y * q.W * Z -
    2.0f * q.Z * q.W * Y +
    q.X * q.X * X +
    2.0f * q.Y * q.X * Y +
    2.0f * q.Z * q.X * Z -
    q.Z * q.Z * X -
    q.Y * q.Y * X;
    
    c2.Y =
    2.0f * q.X * q.Y * X +
    q.Y * q.Y * Y +
    2.0f * q.Z * q.Y * Z +
    2.0f * q.W * q.Z * X -
    q.Z * q.Z * Y +
    q.W * q.W * Y -
    2.0f * q.X * q.W * Z -
    q.X * q.X * Y;
    
    c2.Z =
    2.0f * q.X * q.Z * X +
    2.0f * q.Y * q.Z * Y +
    q.Z * q.Z * Z -
    2.0f * q.W * q.Y * X -
    q.Y * q.Y * Z +
    2.0f * q.W * q.X * Y -
    q.X * q.X * Z +
    q.W * q.W * Z;
    
    return c2;
}

Coord Coord::operator +=(const Coord& c2)
{
    *this = *this + c2;
    return *this;
}

Coord Coord::operator *=(const Coord& c2)
{
    *this = *this * c2;
    return *this;
}

Coord Coord::operator *=(const PrimQuat& c2)
{
    *this = *this * c2;
    return *this;
}

Coord Coord::operator =(const Coord& c2)
{
    X = c2.X;
    Y = c2.Y;
    Z = c2.Z;
    
    return *this;
}

bool Coord::operator==(const Coord &c2)
{
    if (X == c2.X && Y == c2.Y && Z == c2.Z)
        return true;
    return false;
}

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