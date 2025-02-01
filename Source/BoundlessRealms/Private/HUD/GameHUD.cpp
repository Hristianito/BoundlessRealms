#include "HUD/GameHUD.h"
#include "HUD/HUDOverlay.h"

void AGameHUD::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController && HUDOverlayClass)
		{
			HUDOverlay = CreateWidget<UHUDOverlay>(PlayerController, HUDOverlayClass);
			HUDOverlay->AddToViewport();
		}
	}
}
