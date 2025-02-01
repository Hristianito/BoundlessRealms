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

	AEnemy();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	// </AActor>
	
	// <IHitInterface>
	virtual void GetHit(const FVector& HitLocation, AActor* Hitter) override;
	// </IHitInterface>

	bool bCanBeTraced = true;

protected:

	// <AActor>
	virtual void BeginPlay() override;
	// </AActor>

	// <ABaseCharacter>
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual void ReceiveDamage(const float Damage) override;
	virtual void Death() override;
	// </ABaseCharacter>

	UPROPERTY(BlueprintReadOnly)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

private:

	// AI Behaviour
	bool InTargetRange(AActor* Target, double Radius);
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsDead();
	bool IsPatrolling();
	bool IsChasing();
	bool IsAttacking();
	bool IsEngaged();
	void InitializeEnemy();
	void SpawnDefaultWeapon();
	void CheckPatrolTarget();
	void CheckCombatTarget();
	void StartPatrolling();
	void StartChasing();
	void StartAttacking();
	void LoseInterest();
	void ShowHealthBar();
	void HideHealthBar();
	void SetHealthBarPercent();
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void PatrolTimerFinished();
	void ClearPatrolTimer();
	void ClearAttackTimer();

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);

	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent* HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY()
	AAIController* EnemyController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AWeapon> WeaponClass;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	FTimerHandle PatrolTimer;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 4.f;

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMax = 8.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float PatrolSpeed = 125.f;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 200.f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float ChaseSpeed = 300.f;

	UPROPERTY(EditAnywhere)
	float CombatRadius = 500.f;

	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimerMin = 0.5;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackTimerMax = 1.0;

	UPROPERTY(EditAnywhere)
	float AttackRadius = 200.f;
	
	UPROPERTY(EditAnywhere, Category = "Combat")
	float DeathLifeSpan = 5.f;
};
