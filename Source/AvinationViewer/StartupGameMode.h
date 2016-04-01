// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once

#include "Http.h"
#include "GameFramework/GameMode.h"
#include "StartupGameMode.generated.h"
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FLoginEvent, const FString&, EventName, const FString&, EventMessage);

UCLASS(ClassGroup = LoginScreen, Blueprintable)
class AVINATIONVIEWER_API AStartupGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    UPROPERTY(BlueprintReadOnly)
    FString splashUrl;
    
    UPROPERTY(BlueprintAssignable)
    FLoginEvent LoginEventEmitter;
	
    UFUNCTION(BlueprintCallable, Category = "LoginScreen")
    void LoadStartPage();
    
    UFUNCTION(BlueprintCallable, Category = "LoginScreen")
    void BeginLogin(FString firstName, FString lastName, FString password);
    virtual void HandleMatchIsWaitingToStart() override;
    virtual void HandleMatchHasStarted() override;
    
private:
    void RequestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful);
    void LoginRequestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful);
    void LoadLoginScreen();
    void RealDoConnect(TSharedRef<IHttpRequest> req);
    
    FTimerHandle loadDelayTimer;
    FTimerHandle loginDelayTimer;
};
