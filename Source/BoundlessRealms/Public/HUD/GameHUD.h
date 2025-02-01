#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "GameHUD.generated.h"

class UHUDOverlay;

UCLASS()
class BOUNDLESSREALMS_API AGameHUD : public AHUD
{
	GENERATED_BODY()
	
public:

	FORCEINLINE UHUDOverlay* GetHUDOverlay() const { return HUDOverlay; }

protected:

	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UHUDOverlay> HUDOverlayClass;

	UPROPERTY()
	UHUDOverlay* HUDOverlay;
};
