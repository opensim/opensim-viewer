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
