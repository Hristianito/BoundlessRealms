#include "BaseCharacter.h"
#include "Animation/AnimMontage.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BoundlessRealms/DebugMacros.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	Attributes->Health = 100.f;
	Attributes->MaxHealth = 100.f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseCharacter::GetHit(const FVector& HitLocation, AActor* Hitter)
{
	if (IsAlive()) DirectionalHitReact(Hitter);
	else if (!IsAlive()) Death();

	SetWeaponCollision(ECollisionEnabled::NoCollision);
	PlayHitSound(HitLocation);
	PlayHitParticles(HitLocation);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Attack()
{
	if (CombatTarget && CombatTarget->ActorHasTag(FName("Dead")))
	{
		CombatTarget = nullptr;
	}
}

void ABaseCharacter::ReceiveDamage(const float Damage)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(Damage);
	}
}

void ABaseCharacter::Death()
{
	PlayDeathMontage();

	Tags.Add(FName("Dead"));
}

void ABaseCharacter::DirectionalHitReact(AActor* Hitter)
{
	PlayHitReactionMontage(SelectHitMontageSection(Hitter));
}

void ABaseCharacter::AttackEnd()
{
	CurrentWeapon->bCanTrace = true;
}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponBox())
	{
		CurrentWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		CurrentWeapon->IgnoreActors.Empty();
	}
}

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomSectionFromMontage(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	const int32 Selection = PlayRandomSectionFromMontage(DeathMontage, DeathMontageSections);
	TEnumAsByte<EDeathState> State(Selection);

	if (State < EDeathState::EDS_MAX) DeathState = State;

	return Selection;
}

void ABaseCharacter::PlayHitReactionMontage(const FName& SectionName)
{
	PlaySectionFromMontage(HitReactionMontage, SectionName);
}

void ABaseCharacter::StopAttackMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Stop(0.25f, AttackMontage);
	}
}

FVector ABaseCharacter::GetTranslationWarpTarget()
{
	if (CombatTarget == nullptr) return FVector();

	const FVector CombatTargetLocation = CombatTarget->GetActorLocation();
	const FVector Location = GetActorLocation();

	FVector TargetToMe = (Location - CombatTargetLocation).GetSafeNormal();
	TargetToMe *= WarpTargetDistance;

	return CombatTargetLocation + TargetToMe;
}

FVector ABaseCharacter::GetRotationWarpTarget()
{
	if (CombatTarget)
	{
		return CombatTarget->GetActorLocation();
	}
	return FVector();
}

void ABaseCharacter::PlayHitSound(const FVector& HitLocation)
{
	if (HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, HitLocation);
	}
}

void ABaseCharacter::PlayHitParticles(const FVector& HitLocation)
{
	if (HitParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticles, HitLocation);
	}
}

void ABaseCharacter::DisableMeshCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableCapsuleCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseCharacter::DisableWeaponCollision()
{
	SetWeaponCollision(ECollisionEnabled::NoCollision);
}

double ABaseCharacter::CalculateHitAngle(const FVector& HitLocation)
{
	const FVector Forward = GetActorForwardVector();
	const FVector ImpactLowered(HitLocation.X, HitLocation.Y, GetActorLocation().Z);
	const FVector ToHit = (HitLocation - GetActorLocation()).GetSafeNormal();

	const double CosAngle = FVector::DotProduct(Forward, ToHit);
	double Angle = FMath::Acos(CosAngle);
	Angle = FMath::RadiansToDegrees(Angle);

	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);

	if (CrossProduct.Z < 0)
	{
		return Angle * -1;
	}

	return Angle;
}

FName ABaseCharacter::ChooseHitMontageSection(double& Angle)
{
	if (Angle >= -45.f && Angle <= 45.f) return "HitFromFront";
	else if (Angle > -135.f && Angle < -45.f) return "HitFromLeft";
	else if (Angle >= 135.f && Angle <= -135.f) return "HitFromBack";
	else if (Angle > 45.f && Angle < 135.f) return "HitFromRight";
	else return "HitFromBack";
}

FName ABaseCharacter::SelectHitMontageSection(AActor* Hitter)
{
	double Angle = CalculateHitAngle(Hitter->GetActorLocation());

	return ChooseHitMontageSection(Angle);
}

void ABaseCharacter::PlaySectionFromMontage(UAnimMontage* Montage, const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && Montage)
	{
		AnimInstance->Montage_Play(Montage);
		AnimInstance->Montage_JumpToSection(SectionName, Montage);
	}
}

int32 ABaseCharacter::PlayRandomSectionFromMontage(UAnimMontage* Montage, TArray<FName>& MontagesSectionNames)
{
	if (MontagesSectionNames.Num() <= 0) return -1;

	const int32 MaxSections = MontagesSectionNames.Num();
	const int32 SelectedSection = FMath::RandRange(0, MaxSections - 1);

	PlaySectionFromMontage(Montage, MontagesSectionNames[SelectedSection]);

	return SelectedSection;
}