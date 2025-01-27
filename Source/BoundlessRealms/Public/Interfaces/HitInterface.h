#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "HitInterface.generated.h"

UINTERFACE(MinimalAPI)
class UHitInterface : public UInterface
{
	GENERATED_BODY()
};

class BOUNDLESSREALMS_API IHitInterface
{
	GENERATED_BODY()

public:

	virtual void GetHit(const FVector& HitLocation) = 0;
};
