// Fill out your copyright notice in the Description page of Project Settings.

#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AttributeComponent.h"
#include "BoundlessRealms/DebugMacros.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"
#include "Kismet/KismetSystemLibrary.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"	
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBar"));
	HealthBarWidget->SetupAttachment(GetRootComponent());

	PawnSensing = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensing"));
	PawnSensing->SightRadius = 4000.f;
	PawnSensing->SetPeripheralVisionAngle(45.f);

	GetCharacterMovement()->bOrientRotationToMovement = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	HideHealthBar();

	EnemyController = Cast<AAIController>(GetController());

	MoveToTarget(PatrolTarget);

	if (PawnSensing)
	{
		PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);
	}

	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* Weapon = World->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		CurrentWeapon = Weapon;
	}
}

void AEnemy::MoveToTarget(AActor* Target)
{
	if (EnemyController && Target)
	{
		FAIMoveRequest MoveRequest;

		MoveRequest.SetGoalActor(Target);
		MoveRequest.SetAcceptanceRadius(50.f);

		EnemyController->MoveTo(MoveRequest);
	}
}

void AEnemy::PlayDeathMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);

		const int32 RandomSelectDeath = FMath::RandRange(0, 2);
		FName SectionName = FName();

		switch (RandomSelectDeath)
		{
		case 0:
			SectionName = FName("Death1");
			DeathState = EDeathState::EDS_Death1;
			break;
		case 1:
			SectionName = FName("Death2");
			DeathState = EDeathState::EDS_Death2;
			break;
		case 2:
			SectionName = FName("Death3");
			break;
			DeathState = EDeathState::EDS_Death3;
		default:
			break;
		}

		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}

void AEnemy::PlayAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && AttackMontage)
	{
		AnimInstance->Montage_Play(AttackMontage);
		const int32 RandomSelectAttack = FMath::RandRange(0, 1);
		FName SectionName = FName();
		switch (RandomSelectAttack)
		{
		case 0:
			SectionName = FName("Attack1");
			break;
		case 1:
			SectionName = FName("Attack2");
			break;
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, AttackMontage);
	}
}

void AEnemy::Death()
{
	PlayDeathMontage();

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetLifeSpan(3.f);

	HideHealthBar();
}

void AEnemy::Attack()
{
	PlayAttackMontage();
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const float DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
}

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

AActor* AEnemy::ChoosePatrolTarget()
{
	TArray<AActor*> ValidTargets;

	for (AActor* Target : PatrolTargets)
	{
		if (Target != PatrolTarget) ValidTargets.Add(Target);
	}

	if (ValidTargets.Num() > 0)
	{
		const int32 RandomPatrolTarget = FMath::RandRange(0, ValidTargets.Num() - 1);

		return ValidTargets[RandomPatrolTarget];
	}

	return nullptr;
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	bool ShouldChase =
		EnemyState == EEnemyState::EES_Patrolling &&
		SeenPawn->ActorHasTag(FName("MainCharacter"));

	if (ShouldChase)
	{	
		ClearPatrolTimer();

		CombatTarget = SeenPawn;
		
		StartChasing();
	}
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (IsPatrolling())
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
}

void AEnemy::CheckPatrolTarget()
{
	if (InTargetRange(PatrolTarget, PatrolRadius))
	{
		PatrolTarget = ChoosePatrolTarget();

		const float RandomWaitTime = FMath::RandRange(PatrolWaitMin, PatrolWaitMax);

		GetWorldTimerManager().SetTimer(PatrolTimer, this, &AEnemy::PatrolTimerFinished, RandomWaitTime);
	}
}

void AEnemy::CheckCombatTarget()
{
	if (IsOutsideCombatRadius() && !IsPatrolling())
	{
		ClearAttackTimer();

		LoseInterest();

		if (!IsEngaged()) StartPatrolling();
	}
	else if (IsOutsideAttackRadius() && !IsChasing())
	{
		ClearAttackTimer();

		if (!IsEngaged()) StartChasing();
	}
	else if (CanAttack())
	{
		StartAttacking();
	}
}

bool AEnemy::IsOutsideCombatRadius()
{
	return !InTargetRange(CombatTarget, CombatRadius);
}

bool AEnemy::IsOutsideAttackRadius()
{
	return !InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsInsideAttackRadius()
{
	return InTargetRange(CombatTarget, AttackRadius);
}

bool AEnemy::IsDead()
{
	return EnemyState == EEnemyState::EES_Dead;
}

bool AEnemy::IsPatrolling()
{
	return EnemyState == EEnemyState::EES_Patrolling;
}

bool AEnemy::IsChasing()
{
	return EnemyState == EEnemyState::EES_Chasing;
}

bool AEnemy::IsAttacking()
{
	return EnemyState == EEnemyState::EES_Attacking;
}

bool AEnemy::IsEngaged()
{
	return EnemyState == EEnemyState::EES_Engaged;
}

void AEnemy::StartPatrolling()
{
	EnemyState = EEnemyState::EES_Patrolling;

	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;

	MoveToTarget(PatrolTarget);
}

void AEnemy::StartChasing()
{
	EnemyState = EEnemyState::EES_Chasing;

	GetCharacterMovement()->MaxWalkSpeed = ChaseSpeed;

	MoveToTarget(CombatTarget);
}

void AEnemy::StartAttacking()
{
	EnemyState = EEnemyState::EES_Attacking;

	const float RandomWaitTime = FMath::RandRange(AttackTimerMin, AttackTimerMax);

	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, RandomWaitTime);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsDead();
}

void AEnemy::ReceiveDamage(const float Damage)
{
	Super::ReceiveDamage(Damage);
	
	SetHealthBarPercent();
}

void AEnemy::LoseInterest()
{
	CombatTarget = nullptr;

	HideHealthBar();
}

void AEnemy::ShowHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(true);
	}
}

void AEnemy::HideHealthBar()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetVisibility(false);
	}
}

void AEnemy::SetHealthBarPercent()
{
	if (HealthBarWidget)
	{
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AEnemy::GetHit(const FVector& HitLocation)
{
	ShowHealthBar();

	if (IsAlive()) DirectionalHitReact(HitLocation);
	else if (!IsAlive()) Death();

	PlayHitSound(HitLocation);

	PlayHitParticles(HitLocation);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ReceiveDamage(DamageAmount);

	CombatTarget = EventInstigator->GetPawn();

	StartChasing();

	return DamageAmount;
}

void AEnemy::Destroyed()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
	}
}

