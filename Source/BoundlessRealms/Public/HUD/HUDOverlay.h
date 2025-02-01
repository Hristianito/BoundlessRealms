#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDOverlay.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class BOUNDLESSREALMS_API UHUDOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:

	void SetHealthPercent(float Percent);
	void SetStaminaPercent(float Percent);
	void SetTextCoins(int32 Coins);
	void SetTextSouls(int32 Souls);

private:

	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthProgressBar;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* StaminaProgressBar;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextCoins;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TextSouls;

};
