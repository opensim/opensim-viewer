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
#include <stdio.h>
#include <string.h>
#include "LLSDDecode.h"

#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include "HideWindowsPlatformTypes.h"
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif



LLSDDecode::LLSDDecode()
{
    items = 0;
}

LLSDDecode::~LLSDDecode()
{
    if (items != 0)
        delete items;
}

void LLSDDecode::Reset()
{
    if (items != 0)
        delete items;
    items = 0;
}

bool LLSDDecode::Decode(uint8_t **data)
{
    items = DecodeItem(data);
    
    if (!items)
        return false;
    
    return true;
}

double LLSDDecode::ntohd(double n)
{
    uint32_t upper = *((uint32_t *)&n + 1);
    uint32_t lower = *(uint32_t *)&n;
    
    double ret = 0;
    
    *((uint32_t *)&ret + 1) = ntohl(lower);
    *(uint32_t *)&ret = ntohl(upper);
    
    return ret;
}

LLSDItem *LLSDDecode::DecodeItem(uint8_t **d)
{
    LLSDItem *ret = new LLSDItem();
    uint32_t i;
    
    uint8_t *data = *d;
    
    switch (*data++)
    {
        case '!':
            ret->type = UNDEF;
            break;
        case '1':
            ret->type = LBOOLEAN;
            ret->data.booleanData = true;
            break;
        case '0':
            ret->type = LBOOLEAN;
            ret->data.booleanData = false;
            break;
        case 'i':
            ret->type = INTEGER;
            ret->data.integerData = ntohl(*(long *)data);
            data += 4;
            break;
        case 'r':
            ret->type = DOUBLE;
            ret->data.doubleData = ntohd(*(double *)data);
            data += 8;
            break;
        case 'u':
            ret->type = UUID;
            ret->data.stringData = new char[37];
            sprintf(ret->data.stringData, "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                    data[0],
                    data[1],
                    data[2],
                    data[3],
                    data[4],
                    data[5],
                    data[6],
                    data[7],
                    data[8],
                    data[9],
                    data[10],
                    data[11],
                    data[12],
                    data[13],
                    data[14],
                    data[15]);
            data += 16;
            break;
        case 'b':
            ret->type = BINARY;
            ret->binaryLength = ntohl(*(uint32_t *)data);
            data += 4;
            ret->data.binaryData = new uint8_t[ret->binaryLength];
            memcpy(ret->data.binaryData, data, ret->binaryLength);
            data += ret->binaryLength;
            break;
        case 's':
        case 'k':
            ret->type = STRING;
            ret->binaryLength = ntohl(*(uint32_t *)data);
            data += 4;
            ret->data.stringData = new char[ret->binaryLength + 1];
            ret->data.stringData[ret->binaryLength] = 0;
            memcpy(ret->data.stringData, data, ret->binaryLength);
            data += ret->binaryLength;
            break;
        case 'l':
            ret->type = URI;
            ret->binaryLength = ntohl(*(uint32_t *)data);
            data += 4;
            ret->data.stringData = new char[ret->binaryLength + 1];
            ret->data.stringData[ret->binaryLength] = 0;
            memcpy(ret->data.stringData, data, ret->binaryLength);
            data += ret->binaryLength;
            break;
        case 'd':
            ret->type = DATE;
            ret->data.doubleData = ntohd(*(double *)data);
            data += 8;
            break;
        case '[':
            ret->type = ARRAY;
            ret->binaryLength = ntohl(*(uint32_t *)data);
            data += 4;
            for (i = 0 ; i < ret->binaryLength ; i++)
            {
                LLSDItem *item = DecodeItem(&data);
                if (item == 0)
                    return 0;
                ret->arrayData.Add(item);
            }
            data++;
            break;
        case '{':
            ret->type = MAP;
            
            ret->binaryLength = ntohl(*(uint32_t *)data);
            data += 4;
            for (i = 0 ; i < ret->binaryLength ; i++)
            {
                LLSDItem *kitem = DecodeItem(&data);
                if (kitem == 0)
                    return 0;
                FString key(kitem->data.stringData);
                
                LLSDItem *item = DecodeItem(&data);
                if (item == 0)
                    return 0;
                ret->mapData.Add(key, item);
                delete kitem;
            }
            data++;
            break;
        default:
            printf("Invalid element: %c\n", *(data - 1));
            return 0;
    }
    
    *d = data;
    
    return ret;
}

void LLSDDecode::DebugDump()
{
    if (items == 0)
    {
        printf("The collection is empty\n");
        return;
    }
    
    DumpItem(items);
}

void LLSDDecode::DumpItem(LLSDItem *item, bool indent)
{
    static int level = 0;
    
    int i;
    if (indent)
    {
        for (i = 0 ; i < level ; i++)
            printf("  ");
    }
    
    level++;
    
    switch (item->type)
    {
        case UNDEF:
            printf("UNDEF\n");
            break;
        case LBOOLEAN:
            printf("BOOLEAN: %s\n", item->data.booleanData ? "TRUE" : "FALSE");
            break;
        case INTEGER:
            printf("INTEGER: %ld\n", item->data.integerData);
            break;
        case DOUBLE:
            printf("DOUBLE: %lf\n", item->data.doubleData);
            break;
        case UUID:
            printf("UUID: %s\n", item->data.stringData);
            break;
        case BINARY:
            printf("BINARY: length %d\n", item->binaryLength);
            break;
        case URI:
            printf("URI: %s\n", item->data.stringData);
            break;
        case STRING:
            printf("STRING: %s\n", item->data.stringData);
            break;
        case DATE:
            printf("DATE: %lf\n", item->data.doubleData);
            break;
        case ARRAY:
            printf("ARRAY [\n");
            for (auto it = item->arrayData.CreateConstIterator() ; it ; ++it)
            {
                DumpItem(*it);
            }
            for (i = 0 ; i < level - 1 ; i++)
                printf("  ");
            printf("]\n");
            break;
        case MAP:
            printf("MAP {\n");
            for (auto it = item->mapData.CreateConstIterator() ; it ; ++it)
            {
                for (i = 0 ; i < level ; i++)
                    printf("  ");
                int l = level;
                //level = 0;
                printf("%ls: ", *((*it).Key));
                DumpItem((*it).Value, false);
                //level = l;
            }
            for (i = 0 ; i < level - 1 ; i++)
                printf("  ");
            printf("}\n");
            break;
    }
    level--;
}
