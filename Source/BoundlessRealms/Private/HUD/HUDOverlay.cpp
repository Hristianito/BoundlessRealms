#include "HUD/HUDOverlay.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UHUDOverlay::SetHealthPercent(float Percent)
{
	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percent);
	}
}

void UHUDOverlay::SetStaminaPercent(float Percent)
{
	if (StaminaProgressBar)
	{
		StaminaProgressBar->SetPercent(Percent);
	}
}

void UHUDOverlay::SetTextCoins(int32 Coins)
{
	if (TextCoins)
	{
		const FString String = FString::Printf(TEXT("%d"), Coins);
		const FText Text = FText::FromString(String);
		TextCoins->SetText(Text);
	}
}

void UHUDOverlay::SetTextSouls(int32 Souls)
{
	if (TextSouls)
	{
		const FString String = FString::Printf(TEXT("%d"), Souls);
		const FText Text = FText::FromString(String);
		TextSouls->SetText(Text);
	}
}
