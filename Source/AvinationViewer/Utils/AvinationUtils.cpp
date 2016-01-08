// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "AvinationUtils.h"

const float ONE_OVER_U16_MAX = 1.0f / (float)USHRT_MAX;

AvinationUtils::AvinationUtils()
{
}

AvinationUtils::~AvinationUtils()
{
}

float AvinationUtils::uint16tofloat(uint16_t val, float lower, float upper)
{
    float fval = (float)val * ONE_OVER_U16_MAX;
    float delta = upper - lower;
    fval *= delta;
    fval += lower;
    
    float maxError = delta * ONE_OVER_U16_MAX;
    if (fabs(fval) < maxError)
        fval = 0.0f;
    
    return fval;
}