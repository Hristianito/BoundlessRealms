// Fill out your copyright notice in the Description page of Project Settings.

#include "Items/Item.h"
#include "BoundlessRealms/DebugMacros.h"
#include "Components/SphereComponent.h"

// Sets default values
AItem::AItem()
{
	PrimaryActorTick.bCanEverTick = true;
	ItemMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ItemMeshComponent"));
	RootComponent = ItemMesh;

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	Sphere->SetupAttachment(GetRootComponent());
}

void AItem::BeginPlay()
{
	Super::BeginPlay();

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AItem::OnSphereOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AItem::OnSphereEndOverlap);
}

float AItem::TransformedSin()
{
	return Amplitude * FMath::Sin(RunningTime * Time);
}

float AItem::TransformedCos()
{
	return Amplitude * FMath::Cos(RunningTime * Time);
}

void AItem::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	FString OA = OtherActor->GetName();
	if (GEngine) 
	{	
		GEngine->AddOnScreenDebugMessage(1, 1.f, FColor::Red, OA);
	}
}

void AItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	FString OA = OtherActor->GetName();
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, 1.f, FColor::Blue, OA);
	}
}

void AItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	RunningTime += DeltaTime;

	AddActorWorldRotation(FRotator(0.f, RunningTime / 360, 0.f));
}

