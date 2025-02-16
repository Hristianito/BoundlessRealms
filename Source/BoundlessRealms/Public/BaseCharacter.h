#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/HitInterface.h"
#include "CharacterStates.h"
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
	virtual void GetHit(const FVector& HitLocation, AActor* Hitter) override;
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

	UFUNCTION(BlueprintCallable)
	virtual void AttackEnd();

	UFUNCTION(BlueprintCallable)
	void SetWeaponCollision(ECollisionEnabled::Type CollisionEnabled);

	// Montages
	virtual int32 PlayAttackMontage();
	void PlayHitReactionMontage(const FName& SectionName);
	void PlayDeathReactionMontage(const FName& SectionName);
	void StopAttackMontage();

	UFUNCTION(BlueprintCallable)
	FVector GetTranslationWarpTarget();

	UFUNCTION(BlueprintCallable)
	FVector GetRotationWarpTarget();

	// Effects
	void PlayHitSound(const FVector& HitLocation);
	void PlayHitParticles(const FVector& HitLocation);
	
	// Disable Collision
	void DisableMeshCollision();
	void DisableCapsuleCollision();
	void DisableWeaponCollision();

	UPROPERTY(VisibleAnywhere)
	UAttributeComponent* Attributes;

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	AWeapon* CurrentWeapon;

	UPROPERTY(BlueprintReadOnly)
	AActor* CombatTarget;

	UPROPERTY(BlueprintReadOnly)
	TEnumAsByte<EDeathState> DeathState;

	UPROPERTY(EditAnywhere)
	double WarpTargetDistance = 75.f;

private:

	// Directional Hit And Death
	void DirectionalHitReact(AActor* Hitter);
	double CalculateHitAngle(const FVector& HitLocation);
	FName ChooseHitMontageSection(double& Angle);
	FName SelectHitMontageSection(AActor* Hitter);
	void DirectionalDeathReact(AActor* Hitter);
	EDeathState ChooseDeathState(const FName& HitDirection);

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

	UPROPERTY(EditAnywhere, Category = "Sounds")
	USoundBase* HitSound;

	UPROPERTY(EditAnywhere, Category = "VisualEffects")
	UParticleSystem* HitParticles;
};
