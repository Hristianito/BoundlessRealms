#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PickupInterface.generated.h"

UINTERFACE(MinimalAPI)
class UPickupInterface : public UInterface
{
	GENERATED_BODY()
};

class AItem;
class ASoul;
class ATreasure;

class BOUNDLESSREALMS_API IPickupInterface
{
	GENERATED_BODY()

public:

	virtual void SetOverlappingItem(AItem* Item);
	virtual void AddSouls(ASoul* Soul);
	virtual void AddGold(ATreasure* Treasure);
};
