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
#include "StartupGameMode.h"
#include "utils/RapidXml.h"
#include "Communication/GridInfo.h"

DECLARE_DELEGATE(ConnectTimerDelegate)

void AStartupGameMode::HandleMatchIsWaitingToStart()
{
    UGameUserSettings *s = GEngine->GameUserSettings;
    if (s)
    {
        FIntPoint size = s->GetScreenResolution();
        if (size.X < 1280)
            size.X = 1280;
        if (size.Y < 720)
            size.Y = 720;
        
        s->SetScreenResolution(size);
        s->ConfirmVideoMode();
        s->ApplyNonResolutionSettings();
        s->ApplyResolutionSettings(false);
        s->SaveSettings();
        s->RequestResolutionChange(size.X, size.Y, s->GetFullscreenMode());
    }
    Super::HandleMatchIsWaitingToStart();
//    GEngine->GameViewport->GetWindow()->GetTopmostAncestor()->Maximize(); //->SetSizingRule(ESizingRule::FixedSize);
//    GEngine->GameViewport->GetWindow()->HideWindow();
    splashUrl = TEXT("blui://Content/LocalHTML/Splash.html");
}

void AStartupGameMode::HandleMatchHasStarted()
{
    Super::HandleMatchHasStarted();
}

void AStartupGameMode::LoadLoginScreen()
{
    GetWorldTimerManager().ClearTimer(loadDelayTimer);
    
    TSharedRef<IHttpRequest> req = (&FHttpModule::Get())->CreateRequest();
    //req->SetHeader(TEXT("Authorization"), TEXT("Basic YXNzZXRzOmdqMzI5dWQ="));
    req->SetVerb(TEXT("GET"));
    
    // TODO: Make this customizable
    req->SetURL("https://login.avination.com/get_grid_info");
    req->OnProcessRequestComplete().BindUObject(this, &AStartupGameMode::RequestDone);
    
    req->ProcessRequest();
}

void AStartupGameMode::RequestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
{
    bool failed = false;
    
    // Timeout, network error, et al.
    if (!response.IsValid())
    {
        failed = true;
    }
    else if (EHttpResponseCodes::IsOk(response->GetResponseCode()))
    {
        // Empty reply is a fail
        if (response->GetContentLength() == 0)
        {
            failed = true;
        }
        else
        {
            TArray<uint8_t> dataArray;

            dataArray = response->GetContent();
            dataArray.Add(0);
            
            rapidxml::xml_document<> doc;
            
            try
            {
                doc.parse<0>((char *)(dataArray.GetData()));
            }
            catch (...)
            {
                // TODO: Handle this
            }
            
            rapidxml::xml_node<> *root = doc.first_node();
            if (!root)
            {
                failed = true;
            }
            else
            {
                rapidxml::xml_node<> *login_page = root->first_node("login_page");
                if (!login_page || !login_page->value())
                {
                    failed = true;
                }
                else
                {
                    GridInfo::login_page = ANSI_TO_TCHAR(login_page->value());
                }
            }
        }
        
    }
    else
    {
        // Web server gave an error
        failed = true;
    }

    if (failed)
    {
        LoginEventEmitter.Broadcast(TEXT("PlayFail"), "");
        LoginEventEmitter.Broadcast(TEXT("LoadUrl"), TEXT("blui://Content/LocalHTML/Failure.html"));
    }
    else
    {
        LoginEventEmitter.Broadcast(TEXT("PlayLogin"), "2");
        LoginEventEmitter.Broadcast(TEXT("LoadUrl"), GridInfo::login_page);
    }
}

void AStartupGameMode::LoadStartPage()
{
    GetWorldTimerManager().SetTimer(loadDelayTimer, this, &AStartupGameMode::LoadLoginScreen, 2, false);
}

void AStartupGameMode::BeginLogin(FString firstName, FString lastName, FString password)
{
    LoginEventEmitter.Broadcast(TEXT("StopSound"), TEXT("2"));
    
    LoginEventEmitter.Broadcast(TEXT("LoadUrl"), TEXT("blui://Content/LocalHTML/Connecting.html"));
    
    TSharedRef<IHttpRequest> req = (&FHttpModule::Get())->CreateRequest();
    req->SetVerb(TEXT("POST"));
    
    password = FMD5::HashAnsiString(*password);
    
    TSharedPtr<FJsonObject> j = MakeShareable(new FJsonObject());
    j->SetStringField(TEXT("firstname"), firstName);
    j->SetStringField(TEXT("lastname"), lastName);
    j->SetStringField(TEXT("password"), password);
    
    FString payload;
    
    TSharedRef< TJsonWriter<> > Writer = TJsonWriterFactory<>::Create(&payload);
    FJsonSerializer::Serialize(j.ToSharedRef(), Writer);
    
    req->SetContentAsString(payload);
    
    // TODO: Make this customizable
    req->SetURL("https://login.avination.com/login_advanced.php");

    
    req->OnProcessRequestComplete().BindUObject(this, &AStartupGameMode::LoginRequestDone);
    
    ConnectTimerDelegate d;
    d.BindUObject(this, &AStartupGameMode::RealDoConnect, req);
    
    GetWorldTimerManager().SetTimer(loginDelayTimer, d, 9, false);
}

void AStartupGameMode::RealDoConnect(TSharedRef<IHttpRequest> req)
{
    GetWorldTimerManager().ClearTimer(loginDelayTimer);
    req->ProcessRequest();
}

void AStartupGameMode::LoginRequestDone(FHttpRequestPtr request, FHttpResponsePtr response, bool bWasSuccessful)
{
    bool failed = false;
    
    // Timeout, network error, et al.
    if (!response.IsValid())
    {
        failed = true;
    }
    else if (EHttpResponseCodes::IsOk(response->GetResponseCode()))
    {
        // Empty reply is a fail
        if (response->GetContentLength() == 0)
        {
            failed = true;
        }
        else
        {
            TSharedPtr<FJsonObject> o = MakeShareable(new FJsonObject());
            
            TSharedRef<TJsonReader<TCHAR>> r = TJsonReaderFactory<TCHAR>::Create(response->GetContentAsString());
            
            FJsonSerializer::Deserialize(r, o);
            
            bool success = o->GetBoolField("success");
            if (!success)
                failed = true;
        }
    }
    else
    {
        // Web server gave an error
        failed = true;
    }
    
    if (failed)
    {
        LoginEventEmitter.Broadcast(TEXT("StopSound"), TEXT("2"));

        LoginEventEmitter.Broadcast(TEXT("PlayFail"), "");
        LoginEventEmitter.Broadcast(TEXT("LoadUrl"), TEXT("blui://Content/LocalHTML/LoginFailure.html"));
    }
    else
    {
    }
}
