#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttributeComponent.generated.h"


class ASoul;
class ATreasure;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BOUNDLESSREALMS_API UAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UAttributeComponent();

	// <UActorComponent>
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// </UActorComponent>

	bool IsAlive() const;
	float GetHealthPercent() const;
	void ReceiveDamage(float Damage);
	float GetStaminaPercent() const;
	void UseStamina(float StaminaUsed);
	void RegenStamina(float DeltaTime);
	void AddSouls(int32 SoulValue);
	void AddGold(int32 GoldValue);

	FORCEINLINE int32 GetSouls() const { return Souls; }
	FORCEINLINE int32 GetGold() const { return Gold; }

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Health;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxHealth;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float Stamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float MaxStamina;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float DodgeCost = 15.f;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	float StaminaRegenRate = 10.f;

private:

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Souls = 0;

	UPROPERTY(EditAnywhere, Category = "Actor Attributes")
	int32 Gold = 0;
};
