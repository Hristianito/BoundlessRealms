#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOUNDLESSREALMS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();

	bool IsAlive() const;
	float GetHealthPercent() const;
	void ReceiveDamage(float Damage);

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;
};
