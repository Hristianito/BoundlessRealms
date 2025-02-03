#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterStates.h"
#include "Interfaces/PickupInterface.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AItem;
class ASoul;
class UAnimMontage;
class UHUDOverlay;

UCLASS()
class BOUNDLESSREALMS_API AMainCharacter : public ABaseCharacter, public IPickupInterface
{
	GENERATED_BODY()

public:

	AMainCharacter();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	// </AActor>

	// <ACharacter>
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// </ACharacter>

	// <IHitInterface>
	virtual void GetHit(const FVector& HitLocation, AActor* Hitter) override;
	// </IHitInterface>

	// <IPickupInterface>
	virtual void SetOverlappingItem(AItem* Item) override;
	virtual void AddSouls(ASoul* Soul) override;
	virtual void AddGold(ATreasure* Treasure) override;
	// </IPickupInterface>

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }
	FORCEINLINE EActionState GetActionState() const { return ActionState; }
	FORCEINLINE TEnumAsByte<EDeathState> GetDeathState() const { return DeathState; }

protected:

	// <AActor>
	virtual void BeginPlay() override;
	// </AActor>

	// <ABaseCharacter>
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void AttackEnd() override;
	virtual void Death() override;
	// </ABaseCharacter>

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinnishedAttachingToHand();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UFUNCTION(BlueprintCallable)
	void HitReactionEnd();

	UFUNCTION(BlueprintCallable)
	void DodgeEnd();

	UPROPERTY(BlueprintReadWrite)
	EActionState ActionState = EActionState::EAS_Unoccupied;

private:

	// Initialize
	void InitializeMainCharacter();
	void AddMainMappingContext(APlayerController* PlayerController);
	void InitializeHUDOverlay(APlayerController* PlayerController);

	// Set HUD
	void SetHUDHealth();
	void SetHUDStamina();

	// Input
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Dodge();
	void PlayDodgeAnimMontage();
	void EKeyPressed();

	// Stamina
	bool HasEnoughStamina();
	bool IsStaminaBarFull();
	void RegenStamina(float DeltaTime);

	// Weapon Equipping
	bool CanEquip();
	bool CanUnequip();
	void PickUpWeapon(AWeapon* OverlappingWeapon);
	void EquipUnequip();
	void Equip();
	void Unequip();
	void PlayEquipUnequipMontage(FName SectionName);

	bool IsUnoccupied();

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* DodgeAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* EKeyAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* AttackAction;

	UPROPERTY(VisibleAnywhere)
	USpringArmComponent* SpringArm;

	UPROPERTY(VisibleAnywhere)
	UCameraComponent* Camera;

	UPROPERTY(VisibleInstanceOnly)
	AItem* OverlappingItem;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* EquipUnequipMontage;

	UPROPERTY(EditDefaultsOnly, Category = Montages)
	UAnimMontage* DodgeMontage;

	UPROPERTY()
	UHUDOverlay* HUDOverlay;
	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

};