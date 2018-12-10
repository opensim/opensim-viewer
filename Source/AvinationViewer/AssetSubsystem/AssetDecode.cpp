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
#include <exception>
#include "AssetDecode.h"
#include "Base64.h"

// The reason must be a character string constant. It will be referenced
// long after the throwing routine has exited.
asset_decode_exception::asset_decode_exception(const char *d)
{
    data = d;
}

const char *asset_decode_exception::what() const NOEXCEPT
{
    return "asset_decode_exception";
}

AssetDecode::AssetDecode(TArray<uint8_t> asset)
{
    input.Empty();
    input.Append(asset);
    input.Add(0);

    CommonDecode(input.GetData());
}

AssetDecode::AssetDecode(const uint8_t *data, uint32_t len)
{
    input.Empty();
    input.Append(data, len);
    input.Add(0);
    
    CommonDecode(input.GetData());
}

void AssetDecode::CommonDecode(const uint8_t *data)
{
    doc.parse<0>((char *)data);
    
    rootNode = doc.first_node("AssetBase");
    if (rootNode == 0)
        throw asset_decode_exception("AssetBase missing");
    
    dataNode = rootNode->first_node("Data");
    if (dataNode == 0)
        throw asset_decode_exception("Data missing");
}

AssetDecode::~AssetDecode()
{
    doc.clear();
}

/*
TArray<uint8_t> AssetDecode::AsArray()
{
    TArray<uint8_t> ret;
    ret.Append((uint8_t *)dataNode->value(), dataNode->value_size());
    return ret;
}
 */

FString AssetDecode::AsString()
{
    return FString(dataNode->value());
}

TArray<uint8_t> AssetDecode::AsBase64DecodeArray()
{
    TArray<uint8_t> ret;
    FBase64::Decode(AsString(), ret);
    return ret;
}

/*
FString AssetDecode::AsBase64DecodeString()
{
    FString src = AsString();
    FString ret;
    FBase64::Decode(src, ret);
    return ret;
}
*/
