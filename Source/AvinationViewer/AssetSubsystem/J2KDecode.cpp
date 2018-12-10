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
#include "J2KDecode.h"
#include "ImageUtils.h"
#include "../Utils/MemStream.h"

void showMsg(const char *msg, void *userData)
{
    UE_LOG(LogTemp, Error, TEXT("%s"), *FString(msg));
}

J2KDecode::J2KDecode()
{
    image = 0;
}

J2KDecode::~J2KDecode()
{
    if (image)
        opj_image_destroy(image);
}

bool J2KDecode::Decode(TSharedPtr<TArray<uint8_t>, ESPMode::ThreadSafe> data)
{
    if (data->Num() == 0)
        return 0;
    
    MemStream memstr(data->GetData(), data->Num());
    
    opj_dparameters_t parameters;
    opj_codec_t *codec;
    image = 0;
    opj_stream_t *str;
    
    opj_set_default_decoder_parameters(&parameters);
    
    codec = opj_create_decompress(OPJ_CODEC_J2K);
    if (!codec)
    {
        return false;
    }
    
    OPJ_BOOL success = opj_setup_decoder(codec, &parameters);
    if (!success)
    {
        opj_destroy_codec(codec);
        return false;
    }
    
    str = opj_stream_default_create(true);
    if (!str)
    {
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
        return false;
    }
    
    opj_stream_set_read_function(str, MemStream::read);
    opj_stream_set_seek_function(str, MemStream::seek);
    opj_stream_set_skip_function(str, MemStream::skip);
    opj_stream_set_user_data(str, &memstr, 0);
    opj_stream_set_user_data_length(str, data->Num());
    //opj_set_info_handler(codec, showMsg, 0);
    //opj_set_warning_handler(codec, showMsg, 0);
    //opj_set_error_handler(codec, showMsg, 0);
    
    success = opj_read_header(str, codec, &image);
    if (!success)
    {
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
        return false;
    }
    
    success = opj_decode(codec, str, image);
    if (!success)
    {
        if (image)
            opj_image_destroy(image);
        image = 0;
        opj_destroy_codec(codec);
        opj_stream_destroy(str);
        return false;
    }
    
    //    printf("NumComps %d ColorSpace %d\n", image->numcomps, image->color_space);
    
    //printf("%d %d %d %d\n", image->comps[0].w, image->comps[0].h, image->comps[0].bpp, image->comps[0].factor);
    
    opj_destroy_codec(codec);
    opj_stream_destroy(str);
    
    if (!image)
        return false;
    
    return true;
}
