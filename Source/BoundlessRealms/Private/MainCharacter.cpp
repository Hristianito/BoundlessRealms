// Fill out your copyright notice in the Description page of Project Settings.


#include "MainCharacter.h"	
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Animation/AnimMontage.h"

// Sets default values
AMainCharacter::AMainCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);
}

// Called when the game starts or when spawned
void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Add Main Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller)) 
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer())) 
		{
			Subsystem->AddMappingContext(MappingContext, 0);
		}
	}
}

void AMainCharacter::Move(const FInputActionValue& Value)
{
	if (ActionState == EActionState::EAS_Unoccupied) 
	{
		const FVector2D MovementVector = Value.Get<FVector2D>();
		
		const FRotator ControlRotation = GetControlRotation();
		const FRotator YawRotation(0.f, ControlRotation.Yaw, 0.f);

		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MovementVector.Y);

		const FVector RigthDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RigthDirection, MovementVector.X);
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D LookVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
	
}

void AMainCharacter::EKeyPressed()
{
	// if there is an overlapping weapon - pick it up
	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		PickUpWeapon(OverlappingWeapon);
	}
	// if you currently have a weapon - equip/unequip it
	else if (CurrentWeapon)
	{
		EquipUnequip();
	}
}

void AMainCharacter::PickUpWeapon(AWeapon* OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"));
	CharacterState = ECharacterState::ECS_EquippedSword;
	OverlappingItem = nullptr;
	CurrentWeapon = OverlappingWeapon;
}

void AMainCharacter::Attack()
{
	if (CanAttack())
	{
		PlayAttackMontage();
		ActionState = EActionState::EAS_Attacking;
	}
}

bool AMainCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied && 
		CharacterState != ECharacterState::ECS_Unequipped;
}

void AMainCharacter::EquipUnequip()
{
	if (CanEquip())
	{
		PlayEquipUnequipMontage(FName("Equip"));
		CharacterState = ECharacterState::ECS_EquippedSword;
		ActionState = EActionState::EAS_EquippingWeapon;
	}
	else if (CanUnequip())
	{
		PlayEquipUnequipMontage(FName("Unequip"));
		CharacterState = ECharacterState::ECS_Unequipped;
		ActionState = EActionState::EAS_EquippingWeapon;
	}
}

bool AMainCharacter::CanEquip()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState == ECharacterState::ECS_Unequipped &&
		CurrentWeapon;
}

bool AMainCharacter::CanUnequip()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void AMainCharacter::PlayAttackMontage()
{
	if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
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

void AMainCharacter::PlayEquipUnequipMontage(FName SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && EquipUnequipMontage)
	{
		AnimInstance->Montage_Play(EquipUnequipMontage);
		AnimInstance->Montage_JumpToSection(SectionName, EquipUnequipMontage);
	}
}

void AMainCharacter::AttackEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::Unequip()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AMainCharacter::Equip()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AMainCharacter::FinnishedEquipping()
{
	ActionState = EActionState::EAS_Unoccupied;
}

// Called every frame
void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Bind functionality to input
void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AMainCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMainCharacter::Attack);
	}
}

