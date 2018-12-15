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
#include "AvnCharacter.h"

// DEBUG FPS
int  FPS;

// Sets default values
AAvnCharacter::AAvnCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
//    collisionComponent = CreateDefaultSubobject<UCapsuleComponent>(TEXT("PLAYER"));
  
    UCapsuleComponent* capsuleComponent = GetCapsuleComponent();
    capsuleComponent->CanCharacterStepUpOn = ECB_Yes;
    //capsuleComponent->bShouldUpdatePhysicsVolume = true;
    //capsuleComponent->bCanEverAffectNavigation = false;
    capsuleComponent->bDynamicObstacle = true;
    capsuleComponent->SetCollisionProfileName(TEXT("Pawn"));
    capsuleComponent->InitCapsuleSize(50.f, 180.0f);

    SetActorLocation(FVector(12800.0,-12800,4000.0));
    UCharacterMovementComponent* CMovement =  GetCharacterMovement();
    CMovement->bCheatFlying = true;


    bAddDefaultMovementBindings = true;
    currentKeysFlag = 0;

    BaseTurnRate = 45.f;
    BaseLookUpRate = 45.f;
}

// Called when the game starts or when spawned
void AAvnCharacter::BeginPlay()
{
	Super::BeginPlay();
    UCharacterMovementComponent* CMovement = GetCharacterMovement();
    CMovement->bCheatFlying = true;
    CMovement->SetMovementMode(MOVE_Flying);
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

    // add some debug display

    FTimerHandle FPSTimerHandle;
    GetWorld()->GetTimerManager().SetTimer(FPSTimerHandle, this, &AAvnCharacter::ShowFrameRate, 1.f, true);
	
}

// Called every frame
void AAvnCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    FPS++;
}

void InitializeDefaultInputBindings()
{
    static bool bBindingsAdded = false;
    if (!bBindingsAdded)
    {
        bBindingsAdded = true;

        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::W, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::S, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Up, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Down, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveForward", EKeys::Gamepad_LeftY, 1.f));

        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::A, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::D, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveRight", EKeys::Gamepad_LeftX, 1.f));

        // HACK: Android controller bindings in ini files seem to not work
        //  Direct overrides here some to work
#if !PLATFORM_ANDROID
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Gamepad_LeftThumbstick, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Gamepad_RightThumbstick, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Gamepad_FaceButton_Bottom, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::LeftControl, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::E, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Q, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::PageUp, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::PageDown, -1.f));
#else
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Gamepad_LeftTriggerAxis, -0.5f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("MoveUp", EKeys::Gamepad_RightTriggerAxis, 0.5f));
#endif

        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TurnRate", EKeys::Gamepad_RightX, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TurnRate", EKeys::Left, -1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("TurnRate", EKeys::Right, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("Turn", EKeys::MouseX, 1.f));

        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookUpRate", EKeys::Gamepad_RightY, 1.f));
        UPlayerInput::AddEngineDefinedAxisMapping(FInputAxisKeyMapping("LookUp", EKeys::MouseY, -1.f));

        UPlayerInput::AddEngineDefinedActionMapping(FInputActionKeyMapping("Quit", EKeys::Escape));
    }
}
// Called to bind functionality to input
void AAvnCharacter::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{

	Super::SetupPlayerInputComponent(InputComponent);

    if (bAddDefaultMovementBindings)
    {
        InitializeDefaultInputBindings();
        // set up gameplay key bindings
        InputComponent->BindAxis("MoveForward", this, &AAvnCharacter::MoveForward);
        InputComponent->BindAxis("MoveRight", this, &AAvnCharacter::MoveRight);
        InputComponent->BindAxis("MoveUp", this, &AAvnCharacter::MoveUp);
        InputComponent->BindAxis("Turn", this, &AAvnCharacter::AddControllerYawInput);
        InputComponent->BindAxis("TurnRate", this, &AAvnCharacter::TurnAtRate);
        InputComponent->BindAxis("LookUp", this, &AAvnCharacter::AddControllerPitchInput);
        InputComponent->BindAxis("LookUpRate", this, &AAvnCharacter::LookUpAtRate);

        InputComponent->BindAction("Quit", IE_Pressed, this, &AAvnCharacter::Quit);
    }
}

void AAvnCharacter::MoveForward(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is forward
        FRotator Rotation = Controller->GetControlRotation();
        // Limit pitch when walking or falling
        if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
        {
            Rotation.Pitch = 0.0f;
        }
        // add movement in that direction
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
        AddMovementInput(Direction, Value);
    }
}


void AAvnCharacter::MoveRight(float Value)
{
    if ((Controller != NULL) && (Value != 0.0f))
    {
        // find out which way is right
        const FRotator Rotation = Controller->GetControlRotation();
        const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
        // add movement in that direction
        AddMovementInput(Direction, Value);
    }
}

void AAvnCharacter::MoveUp(float Value)
{
    if (Value != 0.0f)
    {
        AddMovementInput(FVector::UpVector, Value);
    }
}

void AAvnCharacter::TurnAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

void AAvnCharacter::LookUpAtRate(float Rate)
{
    // calculate delta for this frame from the rate information
    AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds() * CustomTimeDilation);
}

void AAvnCharacter::Quit()
{
    FGenericPlatformMisc::RequestExit(false);
}

void AAvnCharacter::ShowFrameRate()
{
    FVector pos = GetActorLocation();
    GEngine->AddOnScreenDebugMessage(0, 1.f, FColor::Yellow, "FPS: " + FString::FromInt(FPS) +
        " <" + FString::SanitizeFloat(pos.X / 100.0) + "," + FString::SanitizeFloat(-pos.Y / 100.0) +
        "," + FString::SanitizeFloat(pos.Z / 100.0) + ">");
    FPS = 0;
}


