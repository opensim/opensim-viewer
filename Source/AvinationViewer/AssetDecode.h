// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include "RapidXml.h"

/**
 * 
 */

class asset_decode_exception : public std::exception
{
public:
    asset_decode_exception(const char *d);
    virtual const char *what() const noexcept override;
    const char *data;
};

class AVINATIONVIEWER_API AssetDecode
{
public:
	AssetDecode(TArray<uint8_t> asset);
	~AssetDecode();
    
    TArray<uint8_t> AsArray();
    FString AsString();
    TArray<uint8_t> AsBase64DecodeArray();
    FString AsBase64DecodeString();
private:
    TArray<uint8_t>input;
    rapidxml::xml_document<> doc;
    rapidxml::xml_node<> *rootNode;
    rapidxml::xml_node<> *dataNode;
};
