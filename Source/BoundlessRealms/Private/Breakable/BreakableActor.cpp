#include "Breakable/BreakableActor.h"
#include "GeometryCollection/GeometryCollectionComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Items/Treasure.h"
#include "Components/CapsuleComponent.h"

ABreakableActor::ABreakableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	GeometryCollection = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("GeometryCollection"));
	SetRootComponent(GeometryCollection);
	GeometryCollection->SetGenerateOverlapEvents(true);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GeometryCollection->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	Capsule->SetupAttachment(GetRootComponent());
	Capsule->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	Capsule->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Block);
}

void ABreakableActor::GetHit(const FVector& HitLocation, AActor* Hitter)
{
	if (bBroken) return;

	bBroken = true;

	PlayBreakSound();
}

void ABreakableActor::SpawnTreasureRandomly()
{
	int32 RandonDropValue = FMath::RandRange(1, 100);

	if (RandonDropValue < DropPercentage)
	{
		SpawnTreasure();
	}
}

bool ABreakableActor::CanSpawnTreasure(UWorld* World)
{
	return World && TreasureClasses.Num() > 0;
}

void ABreakableActor::SpawnTreasure()
{
	UWorld* World = GetWorld();

	if (CanSpawnTreasure(World))
	{
		FVector Location = GetActorLocation();
		Location.Z += 75.f;

		int32 Selection = FMath::RandRange(0, TreasureClasses.Num() - 1);
		World->SpawnActor<ATreasure>(TreasureClasses[Selection], Location, GetActorRotation());
	}
}

void ABreakableActor::PlayBreakSound()
{
	if (BreakSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, BreakSound, GetActorLocation());
	}
}