#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Treasure.generated.h"

class USoundBase;

UCLASS()
class BOUNDLESSREALMS_API ATreasure : public AItem
{
	GENERATED_BODY()
	
public:

	FORCEINLINE int32 GetGoldValue() const { return GoldValue; }

protected:

	// <AItem>
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	// </AItem>

private:

	UPROPERTY(EditAnywhere, Category = "Treasure Properties")
	int32 GoldValue;
};
