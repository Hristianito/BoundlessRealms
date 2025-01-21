// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "BaseCharacter.generated.h"

class AWeapon;
class UAttributeComponent;
class UAnimMontage;

UCLASS()
class BOUNDLESSREALMS_API ABaseCharacter : public ACharacter, public IHitInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetHit(const FVector& HitLocation) override;

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	void DirectionalHitReact(const FVector& HitLocation);

protected:


	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Attack();

	virtual bool CanAttack();

	virtual void Death();

	bool IsAlive();

	virtual void ReceiveDamage(const float Damage);

	/*
	* Animation Montages
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	/*
	* Animation Functions
	*/

	virtual void PlayAttackMontage();

	void PlayHitReactionMontage(const FName& SectionName);

	/*
	* Animation Notify Functions
	*/

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* CurrentWeapon;

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	/*
	* Hit Effects
	*/

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;

	void PlayHitSound(const FVector& HitLocation);

	void PlayHitParticles(const FVector& HitLocation);
};
