// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "openjpeg.h"
#include <stdlib.h>
#include <unistd.h>

class MemStream
{
public:
    MemStream(uint8_t *buffer, OPJ_SIZE_T length);
    ~MemStream();
    
    static OPJ_SIZE_T inline read(void *buf, OPJ_SIZE_T len, void *me) { return ((MemStream *)me)->read(buf, len); }
    static OPJ_OFF_T inline skip(OPJ_OFF_T len, void *me) { return ((MemStream *)me)->skip(len); }
    static OPJ_BOOL inline seek(OPJ_OFF_T pos, void *me) { return ((MemStream *)me)->seek(pos); }
    static void inline free(void *buf) { }
    
private:
    uint8_t *data;
    OPJ_SIZE_T length;
    OPJ_OFF_T offset;
    
    OPJ_SIZE_T read(void *buf, OPJ_SIZE_T len);
    OPJ_OFF_T skip(OPJ_OFF_T len);
    bool seek(OPJ_OFF_T pos);
};