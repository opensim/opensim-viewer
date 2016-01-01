// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "MemStream.h"
#include <string.h>

MemStream::MemStream(uint8_t *buffer, size_t length)
{
    data = buffer;
    this->length = length;
    offset = 0;
}

MemStream::~MemStream()
{
}


size_t MemStream::read(void *buf, size_t len)
{
    if (offset + len > length)
        len = length - offset;
    
    if (len <= 0)
        return 0;
    
    memcpy(buf, data + offset, len);
    
    offset += len;
    
    return len;
}

off_t MemStream::skip(off_t len)
{
    if (len < 0)
        return -1;
    
    if (offset + len > length)
        len = length - offset;
    
    offset += len;
    
    return len;
}

bool MemStream::seek(off_t pos)
{
    if (pos < 0)
        return OPJ_FALSE;
    
    if (pos > length)
        return OPJ_FALSE;
    
    offset = pos;
    
    return OPJ_TRUE;
}