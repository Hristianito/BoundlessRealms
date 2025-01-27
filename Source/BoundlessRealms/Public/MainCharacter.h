#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "CharacterStates.h"
#include "MainCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AItem;
class UAnimMontage;

UCLASS()
class BOUNDLESSREALMS_API AMainCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:

	AMainCharacter();

	// <AActor>
	virtual void Tick(float DeltaTime) override;
	// </AActor>

	// <ACharacter>
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// </ACharacter>

	// <IHitInterface>
	virtual void GetHit(const FVector& HitLocation) override;
	// </IHitInterface>

	FORCEINLINE void SetOverlappingItem(AItem* Item) { OverlappingItem = Item; }

	FORCEINLINE ECharacterState GetCharacterState() const { return CharacterState; }

protected:

	// <AActor>
	virtual void BeginPlay() override;
	// </AActor>

	// <ABaseCharacter>
	virtual bool CanAttack() override;
	virtual void Attack() override;
	virtual void AttackEnd() override;
	// </ABaseCharacter>

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToHand();

	UFUNCTION(BlueprintCallable)
	void FinnishedAttachingToHand();

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToBack();

	UPROPERTY(BlueprintReadWrite)
	EActionState ActionState = EActionState::EAS_Unoccupied;

private:

	// Input
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void EKeyPressed();

	// Weapon Equipping
	bool CanEquip();
	bool CanUnequip();
	void PickUpWeapon(AWeapon* OverlappingWeapon);
	void EquipUnequip();
	void Equip();
	void Unequip();
	void PlayEquipUnequipMontage(FName SectionName);

	UPROPERTY(EditAnywhere, Category = Input)
	UInputMappingContext* MappingContext;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* MovementAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = Input)
	UInputAction* JumpAction;

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

	ECharacterState CharacterState = ECharacterState::ECS_Unequipped;

};