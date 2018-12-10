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
#include "LLSDMeshDecode.h"
#include "../Utils/ZlibDecompress.h"
#include "LLSDDecode.h"

LLSDMeshDecode::LLSDMeshDecode()
{
}

LLSDMeshDecode::~LLSDMeshDecode()
{
}

LLSDItem *LLSDMeshDecode::Decode(uint8_t *data, FString lod)
{
    LLSDDecode dec;
    
    uint8_t *d = data;
    
    dec.Decode(&d);
    
    TMap<FString, LLSDItem *> high_lod = dec.items->mapData[lod]->mapData;
    
    uint32_t offset = high_lod[TEXT("offset")]->data.integerData;
    uint32_t size = high_lod[TEXT("size")]->data.integerData;
    
    //    UE_LOG(LogTemp, Warning, TEXT("High LOD data at %lu, length %lu"), offset, size);
    
    uint32_t header_length = d - data;
    
    
    uint8_t *outbuf = 0;
    
    int ret = ZlibDecompress::inflate(d + offset, size, &outbuf);
    
    //    UE_LOG(LogTemp, Warning, TEXT("ret = %d"), ret);
    
    if (ret <= 0)
        return 0;
    
    d = outbuf;
    
    dec.Reset();
    
    dec.Decode(&d);
    
    LLSDItem *retval = dec.items;
    
    dec.items = 0;
    
    return retval;
}
