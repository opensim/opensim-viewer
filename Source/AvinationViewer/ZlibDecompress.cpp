// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "ZlibDecompress.h"
#include <zlib.h>

ZlibDecompress::ZlibDecompress()
{
}

ZlibDecompress::~ZlibDecompress()
{
}

int ZlibDecompress::inflate(const uint8_t *src, uint32_t srcLen, uint8_t **dst)
{
    uint32_t dstLen = 16384;
    *dst = (uint8_t *)malloc(dstLen);
    z_stream strm  = {0};
    strm.total_in  = 0;
    strm.avail_in  = srcLen;
    strm.total_out = 0;
    strm.avail_out = dstLen;
    strm.next_in   = (Bytef *) src;
    strm.next_out  = (Bytef *) *dst;
    uint32_t dstsize = dstLen;
    
    strm.zalloc = Z_NULL;
    strm.zfree  = Z_NULL;
    strm.opaque = Z_NULL;
    
    int err = -1;
    int ret = -1;
    
    err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
    if (err == Z_OK)
    {
        while (1)
        {
            err = ::inflate(&strm, Z_FINISH);
            if (err == Z_STREAM_END)
            {
                ret = strm.total_out;
                break;
            }
            else if (err == Z_OK)
            {
                continue;
            }
            else if (err == Z_BUF_ERROR)
            {
                int off = (uint8_t *)strm.next_out - *dst;
                *dst = (uint8_t *)realloc(*dst, dstsize + dstLen);
                dstsize += dstLen;
                strm.next_out = (Bytef *)(*dst + off);
                strm.avail_out += dstLen;
                continue;
            }
            else
            {
                inflateEnd(&strm);
                return err;
            }
        }
    }
    else
    {
        inflateEnd(&strm);
        return err;
    }
    
    inflateEnd(&strm);
    return ret;
}
