// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "openjpeg.h"
#include <stdlib.h>
#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include <io.h>
#include "HideWindowsPlatformTypes.h"
#else
#include <unistd.h>
#endif

class MemStream
{
public:
    MemStream(uint8_t *buffer, OPJ_SIZE_T length);
    ~MemStream();

//    typedef OPJ_BOOL(*opj_stream_seek_fn) (OPJ_OFF_T p_nb_bytes, void * p_user_data);
  
    static size_t inline read(void *buf, size_t len, void *me) { return ((MemStream *)me)->read(buf, len); }
    static OPJ_OFF_T inline skip(OPJ_OFF_T  len, void *me) { return ((MemStream *)me)->_skip(len); }
    static OPJ_BOOL inline seek(OPJ_OFF_T  pos, void *me) { return ((MemStream *)me)->_seek(pos); }
    static void inline free(void *buf) { }
    
private:
    uint8_t *data;
    off_t  length;
    off_t  offset;
    
    size_t read(void *buf, size_t len);
    off_t  _skip(off_t  len);
    bool _seek(off_t  pos);
};