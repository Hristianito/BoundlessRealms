// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "Animation/AnimMontage.h"
#include "Items/Weapons/Weapon.h"
#include "Components/BoxComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABaseCharacter::ABaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));
	Attributes->Health = 100.f;
	Attributes->MaxHealth = 100.f;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
}

// Called when the game starts or when spawned
void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void ABaseCharacter::Attack()
{
}

bool ABaseCharacter::CanAttack()
{
	return false;
}

void ABaseCharacter::Death()
{
}

bool ABaseCharacter::IsAlive()
{
	return Attributes && Attributes->IsAlive();
}

void ABaseCharacter::ReceiveDamage(const float Damage)
{
	if (Attributes)
	{
		Attributes->ReceiveDamage(Damage);
	}
}

void ABaseCharacter::DisableCapsuleCollision()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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

int32 ABaseCharacter::PlayAttackMontage()
{
	return PlayRandomSectionFromMontage(AttackMontage, AttackMontageSections);
}

int32 ABaseCharacter::PlayDeathMontage()
{
	return PlayRandomSectionFromMontage(DeathMontage, DeathMontageSections);
}

void ABaseCharacter::PlayHitReactionMontage(const FName& SectionName)
{
	PlaySectionFromMontage(HitReactionMontage, SectionName);
}

void ABaseCharacter::AttackEnd()
{
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

// Called every frame
void ABaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseCharacter::GetHit(const FVector& HitLocation)
{
}

void ABaseCharacter::SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled)
{
	if (CurrentWeapon && CurrentWeapon->GetWeaponBox())
	{
		CurrentWeapon->GetWeaponBox()->SetCollisionEnabled(CollisionEnabled);
		CurrentWeapon->IgnoreActors.Empty();
	}
}

void ABaseCharacter::DirectionalHitReact(const FVector& HitLocation)
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
		Angle *= -1;
	}

	FName SectionSelect("HitFromBack");

	if (Angle >= -45.f && Angle <= 45.f) SectionSelect = "HitFromFront";
	else if (Angle > -135.f && Angle < -45.f) SectionSelect = "HitFromLeft";
	else if (Angle >= 135.f && Angle <= -135.f) SectionSelect = "HitFromBack";
	else if (Angle > 45.f && Angle < 135.f) SectionSelect = "HitFromRight";

	PlayHitReactionMontage(SectionSelect);
}
