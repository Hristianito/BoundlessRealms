#include "Components/AttributeComponent.h"

UAttributeComponent::UAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
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