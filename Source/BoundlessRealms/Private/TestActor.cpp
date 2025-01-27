#include "TestActor.h"

ATestActor::ATestActor()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;
}

void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	AddActorWorldRotation(FRotator(0.f, RunningTime / 360, 0.f));
}

float ATestActor::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * Time);
}

float ATestActor::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * Time);
}


