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

	ABaseCharacter();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	// </AActor>

	// <IHitInterface>
	virtual void GetHit(const FVector& HitLocation) override;
	// </IHitInterface>

protected:

	// <AActor>
	virtual void BeginPlay() override;
	// </AActor>

	// Combat
	bool IsAlive();
	virtual bool CanAttack();
	virtual void Attack();
	virtual void ReceiveDamage(const float Damage);
	virtual void Death();
	void DirectionalHitReact(const FVector& HitLocation);

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	// Montages
	virtual int32 PlayAttackMontage();
	virtual int32 PlayDeathMontage();
	void PlayHitReactionMontage(const FName& SectionName);

	// Effects
	void PlayHitSound(const FVector& HitLocation);
	void PlayHitParticles(const FVector& HitLocation);
	
	void DisableCapsuleCollision();

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* CurrentWeapon;

private:

	// Directional Hit
	double CalculateHitAngle(const FVector& HitLocation);
	FName ChooseHitMontageSection(double& Angle);
	FName SelectHitMontageSection(const FVector& HitLocation);

	// Montages
	void PlaySectionFromMontage(UAnimMontage* Montage, const FName& SectionName);
	int32 PlayRandomSectionFromMontage(UAnimMontage* Montage, TArray<FName>& SectionNames);

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* HitReactionMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Montages")
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> AttackMontageSections;

	UPROPERTY(EditAnywhere, Category = "Montages")
	TArray<FName> DeathMontageSections;

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;
};
