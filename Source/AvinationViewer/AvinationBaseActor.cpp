// Fill out your copyright notice in the Description page of Project Settings.

#include "AvinationViewer.h"
#include "AvinationBaseActor.h"


// Sets default values
AAvinationBaseActor::AAvinationBaseActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAvinationBaseActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AAvinationBaseActor::Tick( float DeltaTime )
{
	Super::Tick( DeltaTime );

}

