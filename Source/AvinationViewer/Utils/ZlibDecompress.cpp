/*
 * Copyright (c) Contributors, http://opensimulator.org/
 * See CONTRIBUTORS.TXT for a full list of copyright holders.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the OpenSimulator Project nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE DEVELOPERS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "AvinationViewer.h"
#include "ZlibDecompress.h"
#include "zlib.h"

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
