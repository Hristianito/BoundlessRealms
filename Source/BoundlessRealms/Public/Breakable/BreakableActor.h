#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/HitInterface.h"
#include "BreakableActor.generated.h"

class UGeometryCollectionComponent;
class USoundBase;
class ATreasure;
class UCapsuleComponent;

UCLASS()
class BOUNDLESSREALMS_API ABreakableActor : public AActor, public IHitInterface
{
	GENERATED_BODY()
	
public:	

	ABreakableActor();

	// <IHitInterface>
	virtual void GetHit(const FVector& HitLocation, AActor* Hitter) override;
	// </IHitInterface>

protected:

	UFUNCTION(BlueprintCallable)
	void SpawnTreasureRandomly();

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

private:

	bool CanSpawnTreasure(UWorld* World);
	void SpawnTreasure();
	void PlayBreakSound();

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	USoundBase* BreakSound;

	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ATreasure>> TreasureClasses;

	bool bBroken = false;

	UPROPERTY(EditAnywhere)
	int32 DropPercentage = 50;
};
