#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item.generated.h"

class USphereComponent;
class UNiagaraComponent;

enum class EItemState : uint8
{
	EIS_Hovering,
	EIS_Equipped
};

UCLASS()
class BOUNDLESSREALMS_API AItem : public AActor
{
	GENERATED_BODY()

public:

	AItem();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	// </AActor>

protected:

	// <AActor>
	virtual void BeginPlay() override;
	// </AActor>

	template<typename T>
	T Avg(T first, T second);

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleAnywhere)
	USphereComponent* Sphere;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* ItemGlow;

	EItemState ItemState = EItemState::EIS_Hovering;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine parameters")
	float Amplitude = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine parameters")
	float Time = 5.f;

private:

	UPROPERTY(VisibleInstanceOnly)
	float RunningTime = 0.f;
};

template<typename T>
inline T AItem::Avg(T first, T second)
{
	return (first + second) / 2;
}