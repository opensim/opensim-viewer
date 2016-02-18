// (c) 2016 Avination Virtual Limited. All rights reserved.

#include "AvinationViewer.h"
#include "StartupGameMode.h"
#include "utils/RapidXml.h"
#include "Communication/GridInfo.h"


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
    splashUrl = TEXT("blui://Content/LocalHTML/Login.html");
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
        LoginEventEmitter.Broadcast(TEXT("StartLogin"), "2");
        LoginEventEmitter.Broadcast(TEXT("LoadUrl"), GridInfo::login_page);
    }
}

void AStartupGameMode::LoadStartPage()
{
    GetWorldTimerManager().SetTimer(loadDelayTimer, this, &AStartupGameMode::LoadLoginScreen, 2, false);
}
