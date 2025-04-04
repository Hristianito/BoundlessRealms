#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"


UCLASS()
class BOUNDLESSREALMS_API ASoul : public AItem
{
	GENERATED_BODY()
	
public:

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	// </AActor>

	FORCEINLINE int32 GetSoulValue() const { return SoulValue; }
	FORCEINLINE void SetSoulValue(int32 CurrentSoulValue) { SoulValue = CurrentSoulValue; }

protected:

	// <AActor>
	virtual void BeginPlay() override;
	// </AActor>

	// <AItem>
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	// </AItem>

private: 

	void LineTrace(const FVector& Start, const FVector& End, TArray<TEnumAsByte<EObjectTypeQuery>>& ObjectTypes, TArray<AActor*, FDefaultAllocator>& ActorsToIgnore, FHitResult& HitResult);
	void MoveSoul(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 SoulValue;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	float DriftRate = 15.f;

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	bool bShowLineDebug = false;

	UPROPERTY()
	double DesiredZ;
};	
