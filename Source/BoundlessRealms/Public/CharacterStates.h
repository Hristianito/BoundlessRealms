#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Unequipped UMETA(DisplayName = "Unequipped"),
	ECS_EquippedSword UMETA(DisplayName = "Equipped Sword")
};

UENUM(BlueprintType)
enum class EActionState : uint8 
{
	EAS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	EAS_Attacking UMETA(DisplayName = "Attacking"),
	EAS_EquippingWeapon UMETA(DisplayName = "Equipping Weapon"),
	EAS_Hit UMETA(DisplayName = "Hit"),
	EAS_Dead UMETA(DisplayName = "Dead"),
	EAS_Dodging UMETA(DisplayName = "Dodging")
};

UENUM(BlueprintType)
enum EDeathState
{
	EDS_DeathFromFront UMETA(DisplayName = "DeathFromFront"),
	EDS_DeathFromBack UMETA(DisplayName = "DeathFromBack"),
	EDS_DeathFromLeft UMETA(DisplayName = "DeathFromLeft"),
	EDS_DeathFromRight UMETA(DisplayName = "DeathFromRight"),

	EDS_MAX UMETA(DisplayName = "DefaultMAX")
};

UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	EES_NoState UMETA(DisplayName = "NoState"),

	EES_Dead UMETA(DisplayName = "Dead"),
	EES_Patrolling UMETA(DisplayName = "Patrolling"),
	EES_Chasing UMETA(DisplayName = "Chasing"),
	EES_Attacking UMETA(DisplayName = "Attacking"),
	EES_Engaged UMETA(DisplayName = "Engaged")
};
