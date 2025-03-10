#include "Items/Soul.h"
#include "Interfaces/PickupInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void ASoul::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveSoul(DeltaTime);
}

void ASoul::BeginPlay()
{
	Super::BeginPlay();

	const FVector Start = GetActorLocation();
	const FVector End = Start - FVector(0.f, 0.f, 2000.f);

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetOwner());

	FHitResult HitResult;

	LineTrace(Start, End, ObjectTypes, ActorsToIgnore, HitResult);

	DesiredZ = HitResult.ImpactPoint.Z + 50.f;
}

void ASoul::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IPickupInterface* PickupInterface = Cast<IPickupInterface>(OtherActor))
	{
		PickupInterface->AddSouls(this);

		SpawnPickupEffect();
		SpawnPickupSound();
		Destroy();
	}
}

void ASoul::LineTrace(const FVector& Start, const FVector& End, TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, TArray<AActor*, FDefaultAllocator>& ActorsToIgnore, FHitResult& HitResult)
{
	UKismetSystemLibrary::LineTraceSingleForObjects(
		this,
		Start,
		End,
		ObjectTypes,
		false,
		ActorsToIgnore,
		bShowLineDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		HitResult,
		true
	);
}

void ASoul::MoveSoul(float DeltaTime)
{
	const double LocationZ = GetActorLocation().Z;
	if (LocationZ > DesiredZ)
	{
		const FVector DeltaLocation = FVector(0.f, 0.f, DriftRate * DeltaTime);
		AddActorWorldOffset(DeltaLocation);
	}
}