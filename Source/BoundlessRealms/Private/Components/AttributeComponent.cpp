#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	RegenStamina(DeltaTime);
}

bool UAttributeComponent::IsAlive() const
{
	return Health > 0.f;
}

float UAttributeComponent::GetHealthPercent() const
{
	return Health / MaxHealth;
}

void UAttributeComponent::ReceiveDamage(float Damage)
{
	Health = FMath::Clamp(Health - Damage, 0, MaxHealth);
}

float UAttributeComponent::GetStaminaPercent() const
{
	return Stamina / MaxStamina;
}

void UAttributeComponent::UseStamina(float StaminaUsed)
{
	Stamina = FMath::Clamp(Stamina - StaminaUsed, 0, MaxStamina);
}

void UAttributeComponent::RegenStamina(float DeltaTime)
{
	Stamina = FMath::Clamp(Stamina + StaminaRegenRate * DeltaTime, 0.f, MaxStamina);
}

void UAttributeComponent::AddSouls(int32 SoulValue)
{
	Souls += SoulValue;
}

void UAttributeComponent::AddGold(int32 GoldValue)
{
	Gold += GoldValue;
}
