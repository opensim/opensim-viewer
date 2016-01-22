// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once
DECLARE_DELEGATE(ProcessDelegate)

struct AssetID
{
    FGuid ID;
    int32_t extraID;

    AssetID()
    {
        ID = FGuid();
        extraID = 0;
    }

    AssetID(FGuid id)
    {
        ID = id;
        extraID = 0;
    }

    AssetID(FGuid id, int32_t extraid)
    {
        ID = id;
        extraID = extraid;
    }
};

/**
 * 
 */
class AVINATIONVIEWER_API AssetBase
{
public:
    
    enum AssetState
    {
        Waiting = 0,
        Processing = 1,
        Failed = 2,
        Done = 3,
        Retrying = 4
    };
    
	AssetBase();
	virtual ~AssetBase();
    
    void inline SetFailed() { if (stageData.IsValid()) stageData->Empty(); state = AssetState::Failed; }
    
    ProcessDelegate decode; // On dedicated thread
    ProcessDelegate preProcess; // On game thread!
    ProcessDelegate mainProcess; // On dedicated thread
    ProcessDelegate postProcess; // On game thread

    FGuid id;
    AssetState state;
protected:
    void Decode();
    
    TSharedPtr<TArray<uint8_t>, ESPMode::ThreadSafe> stageData;
    
    friend class AssetCache;
};

typedef TSharedRef<AssetBase, ESPMode::ThreadSafe> TSharedAssetRef;