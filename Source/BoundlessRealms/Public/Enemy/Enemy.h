// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "CharacterStates.h"
#include "Enemy.generated.h"

class UHealthBarComponent;
class UPawnSensingComponent;
class AAIController;
class AWeapon;

UCLASS()
class BOUNDLESSREALMS_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void CheckPatrolTarget();

	void CheckCombatTarget();

	virtual void GetHit(const FVector& HitLocation) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;


	virtual void Destroyed() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveToTarget(AActor* Target);

	/*
	* Components
	*/

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	/*
	* Animation Functions
	*/

	virtual int32 PlayDeathMontage() override;

	virtual void Death() override;


	virtual void Attack() override;

	bool InTargetRange(AActor* Target, double Radius);

	void PatrolTimerFinished();

	AActor* ChoosePatrolTarget();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	void ClearPatrolTimer();

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathState> DeathState;

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	UPROPERTY(EditAnywhere)
	float CombatRadius = 500.f;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 150.f;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 4.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 8.f;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	/*
	* AI Behaviour
	*/

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolSpeed = 125.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseSpeed = 300.f;

	bool IsOutsideCombatRadius();

	bool IsOutsideAttackRadius();

	bool IsInsideAttackRadius();

	bool IsDead();

	bool IsPatrolling();

	bool IsChasing();

	bool IsAttacking();

	bool IsEngaged();

	void ShowHealthBar();

	void HideHealthBar();

	void SetHealthBarPercent();

	void LoseInterest();

	void StartPatrolling();

	void StartChasing();

	void StartAttacking();

	/*
	* Combat
	*/

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimerMin = 0.5;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimerMax = 1.0;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 5.f;

	FTimerHandle AttackTimer;

	void ClearAttackTimer();

	virtual bool CanAttack() override;

	virtual void ReceiveDamage(const float Damage) override;

private:

	UPROPERTY()
	AActor* CombatTarget;
};
