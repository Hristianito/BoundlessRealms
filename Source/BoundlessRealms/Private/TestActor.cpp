// Fill out your copyright notice in the Description page of Project Settings.
#include "TestActor.h"
#include "BoundlessRealms/DebugMacros.h"

// Sets default values
ATestActor::ATestActor()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
}
void ATestActor::BeginPlay()
{
	Super::BeginPlay();
}
float ATestActor::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * Time);
}
float ATestActor::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * Time);
}
void ATestActor::Tick(float DeltaTime)	
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	AddActorWorldRotation(FRotator(0.f, RunningTime / 360, 0.f));
}

