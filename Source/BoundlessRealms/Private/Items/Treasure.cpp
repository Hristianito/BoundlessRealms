#include "Items/Treasure.h"
#include "MainCharacter.h"
#include "Kismet/GameplayStatics.h"

void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AMainCharacter* MainCharacter = Cast<AMainCharacter>(OtherActor))
	{
		PlayTreasurePickupSound();

		Destroy();
	}
}

void ATreasure::PlayTreasurePickupSound()
{
	if (TreasurePickupSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, TreasurePickupSound, GetActorLocation());
	}
}
