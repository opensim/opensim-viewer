// (c) 2016 Avination Virtual Limited. All rights reserved.

#pragma once

#include "GameFramework/Character.h"
#include "AvnCharacter.generated.h"

UCLASS()
class AVINATIONVIEWER_API AAvnCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAvnCharacter();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;
    //handles moving forward/backward
    UFUNCTION()
    void MoveForward(float Val);
    //handles strafing
    UFUNCTION()
    void MoveRight(float Val);
    UFUNCTION()
    void MoveUp(float Value);
    UFUNCTION()
    void TurnAtRate(float Rate);
    UFUNCTION()
    void LookUpAtRate(float Rate);
    UFUNCTION()
    void Quit();

    void ShowFrameRate();

    float BaseTurnRate;
    float BaseLookUpRate;
    bool bAddDefaultMovementBindings;
    int32_t currentKeysFlag;

};
