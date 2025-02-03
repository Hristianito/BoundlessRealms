#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Soul.generated.h"


UCLASS()
class BOUNDLESSREALMS_API ASoul : public AItem
{
	GENERATED_BODY()
	
public:

	FORCEINLINE int32 GetSoulValue() const { return SoulValue; }
	FORCEINLINE void SetSoulValue(int32 CurrentSoulValue) { SoulValue = CurrentSoulValue; }

protected:

	// <AItem>
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	// </AItem>

private: 

	UPROPERTY(EditAnywhere, Category = "Soul Properties")
	int32 SoulValue;
};	
