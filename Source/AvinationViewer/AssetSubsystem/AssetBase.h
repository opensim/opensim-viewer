// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
DECLARE_DELEGATE(ProcessDelegate)
DECLARE_DELEGATE_OneParam(DecodeDelegate, TArray<uint8_t>&)

/**
 * 
 */
class AVINATIONVIEWER_API AssetBase
{
public:
	AssetBase();
	virtual ~AssetBase();
    
    TSharedPtr<TArray<uint8_t>, ESPMode::ThreadSafe> rawData;

    DecodeDelegate decode; // On dedicated thread
    ProcessDelegate preProcess; // On game thread!
    ProcessDelegate mainProcess; // On dedicated thread
    ProcessDelegate postProcess; // On game thread
    
private:
    void Decode(TArray<uint8_t>& data);
};
