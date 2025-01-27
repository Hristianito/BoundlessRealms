#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestActor.generated.h"

UCLASS()
class BOUNDLESSREALMS_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	

	ATestActor();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	// </AActor>

protected:

	template<typename T>
	T Avg(T first, T second);

	UFUNCTION(BlueprintPure)
	float TransformedSin();

	UFUNCTION(BlueprintPure)
	float TransformedCos();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine parameters")
	float Amplitude = 0.2f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sine parameters")
	float Time = 5.f;

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ItemMesh;

	UPROPERTY(VisibleInstanceOnly)
	float RunningTime = 0.f;
};

template<typename T>
inline T ATestActor::Avg(T first, T second)
{
	return (first + second) / 2;
}
