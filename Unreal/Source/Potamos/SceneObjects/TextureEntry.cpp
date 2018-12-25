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
#include "Potamos.h"
#include "TextureEntry.h"
#if PLATFORM_WINDOWS
#include "AllowWindowsPlatformTypes.h"
#include <winsock2.h>
#include "HideWindowsPlatformTypes.h"
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

TextureEntry::TextureEntry()
{
    color = FLinearColor(1, 1, 1, 1);
    repeatU = 1.0f;
    repeatV = 1.0f;
    offsetU = 0.0f;
    offsetV = 0.0f;
    rotation = 0.0f;
    glow = 0.0f;
    material = 0;
    media = 0;
}
/*
TextureEntry::TextureEntry(TextureEntry& source)
{
    color = source.color;
    repeatU = source.repeatU;
    repeatV = source.repeatV;
    offsetU = source.offsetU;
    offsetV = source.offsetV;
    rotation = source.rotation;
    glow = source.glow;
    material = source.material;
    media = source.media;
    textureId = source.textureId;
    materialId = source.materialId;
}
*/
TextureEntry::~TextureEntry()
{
}

bool TextureEntry::Parse(TArray<uint8_t> in, TextureEntry& defaultEntry, TArray<TextureEntry>& entries)
{
    //UE_LOG(LogTemp, Warning, TEXT("--- Parsing TextureEntry ---"));
    
    entries.Empty();
    entries.AddDefaulted(32);
    
    if (in.Num() < 16)
        return false;
    
    const uint8_t *inData = in.GetData();
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to parse default texture ID"));
    
    // Texture ID //////////////////////////////////
    defaultEntry.textureId = FGuid(ntohl(*(uint32_t *)&in[0]),
                                   ntohl(*(uint32_t *)&in[4]),
                                   ntohl(*(uint32_t *)&in[8]),
                                   ntohl(*(uint32_t *)&in[12]));
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default texture ID %s"), *defaultEntry.textureId.ToString().ToLower());
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].textureId = defaultEntry.textureId;
                    
    uint8_t bitfieldSize = 0;
    uint32_t faceBits = 0;
    const uint8_t *pos = inData + 16;

    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading texture ID bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        FGuid texId = FGuid(ntohl(*(uint32_t *)(pos)),
                            ntohl(*(uint32_t *)(pos + 4)),
                            ntohl(*(uint32_t *)(pos + 8)),
                            ntohl(*(uint32_t *)(pos + 12)));
        
        //UE_LOG(LogTemp, Warning, TEXT("Texture ID for these faces is %s"), *texId.ToString());
        
        pos += 16;
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].textureId = texId;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // Color //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to parse default color"));
    
    defaultEntry.color = FLinearColor(1.0f - (float)*(pos) / 255.0f,
                                      1.0f - (float)*(pos + 1) / 255.0f,
                                      1.0f - (float)*(pos + 2) / 255.0f,
                                      1.0f - (float)*(pos + 3) / 255.0f);
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default color %s"), *defaultEntry.color.ToString());
    
    pos += 4;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].color = defaultEntry.color;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading color bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        FLinearColor col(1.0f - (float)*(pos) / 255.0f,
                         1.0f - (float)*(pos + 1) / 255.0f,
                         1.0f - (float)*(pos + 2) / 255.0f,
                         1.0f - (float)*(pos + 3) / 255.0f);
        
        //UE_LOG(LogTemp, Warning, TEXT("Color for these faces is %s"), *col.ToString());
        
        pos += 4;
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].color = col;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // repeatU //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;

    //UE_LOG(LogTemp, Warning, TEXT("Going to read default repeat U"));
           
    defaultEntry.repeatU = *(float *)pos;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default repeat U: %f"), defaultEntry.repeatU);
    
    pos += 4;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].repeatU = defaultEntry.repeatU;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading repeat U bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        float repeatU = *(float *)pos;
        
        pos += 4;
        
        //UE_LOG(LogTemp, Warning, TEXT("Repeat U for these faces is: %f"), repeatU);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].repeatU = repeatU;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // repeatV //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to read default repeat V"));
    
    defaultEntry.repeatV = *(float *)pos;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default repeat V: %f"), defaultEntry.repeatV);
    
    pos += 4;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].repeatV = defaultEntry.repeatV;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading repeat V bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        float repeatV = *(float *)pos;
        
        pos += 4;
        
        //UE_LOG(LogTemp, Warning, TEXT("Repeat V for these faces is: %f"), repeatV);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].repeatV = repeatV;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // offsetU //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to read default offset U"));

    defaultEntry.offsetU = (float)(*(int16_t *)pos) / 32767.0f;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default offset U: %f"), defaultEntry.offsetU);
    
    pos += 2;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].offsetU = defaultEntry.offsetU;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading offset U bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        float offsetU = (float)(*(int16_t *)pos) / 32767.0f;
        
        pos += 2;
        
        //UE_LOG(LogTemp, Warning, TEXT("Offset U for these faces is: %f"), offsetU);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].offsetU = offsetU;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // offsetV //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to read default offset V"));
    
    defaultEntry.offsetV = (float)(*(int16_t *)pos) / 32767.0f;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default offset V: %f"), defaultEntry.offsetV);
    
    pos += 2;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].offsetV = defaultEntry.offsetV;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading offset V bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        float offsetV = (float)(*(int16_t *)pos) / 32767.0f;
        
        pos += 2;
        
        //UE_LOG(LogTemp, Warning, TEXT("Offset V for these faces is: %f"), offsetV);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].offsetV = offsetV;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // rotation //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to read default rotation"));
    
    defaultEntry.rotation = (float)(*(uint16_t *)pos) / 32767.0f;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default rotation: %f"), defaultEntry.rotation);
    
    pos += 2;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].rotation = defaultEntry.rotation;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading rotation bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        float rotation = (float)(*(uint16_t *)pos) / 32767.0f;
        
        pos += 2;
        
        //UE_LOG(LogTemp, Warning, TEXT("Rotation for these faces is: %f"), rotation);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].rotation = rotation;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // material //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to read default material"));
    
    defaultEntry.material = *pos;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default material: %d"), defaultEntry.material);
    
    pos += 1;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].material = defaultEntry.material;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading material bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        uint8_t material = *pos;
        
        pos += 1;
        
        //UE_LOG(LogTemp, Warning, TEXT("Material for these faces is: %d"), material);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].material = material;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // media //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to read default media"));
    
    defaultEntry.media = *pos;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default media: %d"), defaultEntry.media);
    
    pos += 1;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].media = defaultEntry.media;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading media bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        uint8_t media = *pos;
        
        pos += 1;
        
        //UE_LOG(LogTemp, Warning, TEXT("Media for these faces is: %d"), media);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].media = media;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // glow //////////////////////////////////
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to read default glow"));
    
    defaultEntry.glow = (float)*pos / 255.0f;
    
    //UE_LOG(LogTemp, Warning, TEXT("Read default glow: %f"), defaultEntry.glow);
    
    pos += 1;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].glow = defaultEntry.glow;
    
    if (pos - inData >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading glow bitfield"));
    
    while (ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        float glow = (float)*pos / 255.0f;
        
        pos += 1;
        
        //UE_LOG(LogTemp, Warning, TEXT("Glow for these faces is: %f"), glow);
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].glow = glow;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    // Material ID //////////////////////////////////
    if (pos - inData + 16 >= in.Num())
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Going to parse default material ID"));
    
    defaultEntry.materialId = FGuid(ntohl(*(uint32_t *)(pos)),
                                    ntohl(*(uint32_t *)(pos + 4)),
                                    ntohl(*(uint32_t *)(pos + 8)),
                                    ntohl(*(uint32_t *)(pos + 12)));

    //UE_LOG(LogTemp, Warning, TEXT("Read default material ID %s"), *defaultEntry.materialId.ToString());
    
    pos += 16;
    
    for (int i = 0 ; i < 32 ; i++)
        entries[i].materialId = defaultEntry.materialId;
    
    if (pos - inData + 16 >= in.Num())
    {
        //UE_LOG(LogTemp, Warning, TEXT("Consumed %d of %d bytes"), pos - inData, in.Num());
    }
        return true;
    
    //UE_LOG(LogTemp, Warning, TEXT("Reading material ID bitfield"));
    
    while ((pos - inData + 16 <= in.Num()) && ReadBitfield(pos, faceBits, bitfieldSize))
    {
        //UE_LOG(LogTemp, Warning, TEXT("Bitfield size is %d: %08lx"), bitfieldSize, faceBits);
        
        FGuid materialId = FGuid(ntohl(*(uint32_t *)(pos)),
                                 ntohl(*(uint32_t *)(pos + 4)),
                                 ntohl(*(uint32_t *)(pos + 8)),
                                 ntohl(*(uint32_t *)(pos + 12)));
        pos += 16;
        
        //UE_LOG(LogTemp, Warning, TEXT("Material ID for these faces is %s"), *materialId.ToString());
        
        for (uint8_t face = 0, bit = 1 ; face < bitfieldSize ; face++, bit <<= 1)
        {
            if (faceBits & bit)
                entries[face].materialId = materialId;
        }
    }
    
    //UE_LOG(LogTemp, Warning, TEXT("Done processing bitfield"));
    
    return true;
}

bool TextureEntry::ReadBitfield(const uint8_t*& pos, uint32_t& faceBits, uint8_t& bitfieldSize)
{
    faceBits = 0;
    bitfieldSize = 0;
    
    uint8_t b = 0;
    do
    {
        b = *pos++;
        faceBits = (faceBits << 7) | (b & 0x7f);
        bitfieldSize += 7;
    }
    while (b & 0x80);
    
    return faceBits != 0;
}
