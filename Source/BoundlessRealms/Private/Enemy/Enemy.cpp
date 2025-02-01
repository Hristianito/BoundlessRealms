#include "Enemy/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Items/Weapons/Weapon.h"
#include "HUD/HealthBarComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"	
#include "Navigation/PathFollowingComponent.h"
#include "Perception/PawnSensingComponent.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetGenerateOverlapEvents(true);
	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

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

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsDead()) return;

	if (EnemyState > EEnemyState::EES_Patrolling)
	{
		CheckCombatTarget();
	}
	else
	{
		CheckPatrolTarget();
	}
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

void AEnemy::GetHit(const FVector& HitLocation, AActor* Hitter)
{
	Super::GetHit(HitLocation, Hitter);

	if (!IsDead()) ShowHealthBar();
	ClearPatrolTimer();
	ClearAttackTimer();
	StopAttackMontage();
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	if (PawnSensing) PawnSensing->OnSeePawn.AddDynamic(this, &AEnemy::PawnSeen);

	InitializeEnemy();
	Tags.Add(FName("Enemy"));
}

bool AEnemy::CanAttack()
{
	return IsInsideAttackRadius() && !IsAttacking() && !IsEngaged() && !IsDead();
}

void AEnemy::Attack()
{
	Super::Attack();

	if (CombatTarget == nullptr) return;

	EnemyState = EEnemyState::EES_Engaged;

	PlayAttackMontage();
}

void AEnemy::AttackEnd()
{
	Super::AttackEnd();

	EnemyState = EEnemyState::EES_NoState;

	CheckCombatTarget();
}

void AEnemy::ReceiveDamage(const float Damage)
{
	Super::ReceiveDamage(Damage);

	SetHealthBarPercent();
}

void AEnemy::Death()
{
	Super::Death();

	EnemyState = EEnemyState::EES_Dead;

	ClearAttackTimer();
	DisableCapsuleCollision();
	DisableWeaponCollision();
	SetLifeSpan(DeathLifeSpan);
	HideHealthBar();
	GetCharacterMovement()->bOrientRotationToMovement = false;
}

bool AEnemy::InTargetRange(AActor* Target, double Radius)
{
	if (Target == nullptr) return false;
	const float DistanceToTarget = (Target->GetActorLocation() - GetActorLocation()).Size();
	return DistanceToTarget <= Radius;
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

void AEnemy::InitializeEnemy()
{
	EnemyController = Cast<AAIController>(GetController());
	MoveToTarget(PatrolTarget);
	HideHealthBar();
	SpawnDefaultWeapon();
}

void AEnemy::SpawnDefaultWeapon()
{
	UWorld* World = GetWorld();
	if (World && WeaponClass)
	{
		AWeapon* Weapon = World->SpawnActor<AWeapon>(WeaponClass);
		Weapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
		CurrentWeapon = Weapon;
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
	ShowHealthBar();
	MoveToTarget(CombatTarget);
	ClearPatrolTimer();
}

void AEnemy::StartAttacking()
{
	EnemyState = EEnemyState::EES_Attacking;
	const float RandomWaitTime = FMath::RandRange(AttackTimerMin, AttackTimerMax);
	GetWorldTimerManager().SetTimer(AttackTimer, this, &AEnemy::Attack, RandomWaitTime);
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

void AEnemy::PatrolTimerFinished()
{
	MoveToTarget(PatrolTarget);
}

void AEnemy::ClearPatrolTimer()
{
	GetWorldTimerManager().ClearTimer(PatrolTimer);
}

void AEnemy::ClearAttackTimer()
{
	GetWorldTimerManager().ClearTimer(AttackTimer);
}

void AEnemy::PawnSeen(APawn* SeenPawn)
{
	bool ShouldChase =
		EnemyState == EEnemyState::EES_Patrolling &&
		SeenPawn->ActorHasTag(FName("MainCharacter")) &&
		!SeenPawn->ActorHasTag(FName("Dead"));

	if (ShouldChase)
	{	
		CombatTarget = SeenPawn;
		StartChasing();
	}
}