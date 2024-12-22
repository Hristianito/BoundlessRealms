// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
	ABreakableActor();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit(const FVector& HitLocation) override;

	UFUNCTION(BlueprintCallable)
	void SpawnTreasure();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* Capsule;

private:

	UPROPERTY(VisibleAnywhere)
	UGeometryCollectionComponent* GeometryCollection;

	UPROPERTY(EditAnywhere, Category = "Breakable Properties")
	USoundBase* BreakSound;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<ATreasure>> TreasureClasses;

	bool bBroken = false;
};
