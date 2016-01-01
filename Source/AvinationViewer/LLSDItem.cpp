// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "LLSDItem.h"

LLSDItem::LLSDItem()
{
    type = UNDEF;
}

LLSDItem::~LLSDItem()
{
    if (type == ARRAY)
    {
        for (auto it = arrayData.CreateConstIterator() ; it ; ++it)
        {
            delete (*it);
        }
        
    }
    else if (type == MAP)
    {
        for (auto it = mapData.CreateConstIterator() ; it ; ++it)
        {
            delete (*it).Value;
        }
    }
    else if (type == STRING || type == URI || type == UUID)
    {
        delete data.stringData;
    }
    else if (type == BINARY)
    {
        delete data.binaryData;
    }
}
