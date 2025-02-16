#include "MainCharacter.h"	
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Pawn.h"
#include "Camera/CameraComponent.h"
#include "HUD/GameHUD.h"
#include "HUD/HUDOverlay.h"
#include "Components/InputComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/AttributeComponent.h"
#include "Components/CapsuleComponent.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Items/Item.h"
#include "Items/Weapons/Weapon.h"
#include "Items/Soul.h"
#include "Items/Treasure.h"
#include "Animation/AnimMontage.h"

AMainCharacter::AMainCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldDynamic, ECollisionResponse::ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 300.f;

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(SpringArm);

	Attributes->Stamina = 100.f;
	Attributes->MaxStamina = 100.f;
}

void AMainCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	RegenStamina(DeltaTime);
}

float AMainCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ReceiveDamage(DamageAmount);
	SetHUDHealth();

	return DamageAmount;
}

void AMainCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AMainCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacter::Look);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Triggered, this, &AMainCharacter::Dodge);
		EnhancedInputComponent->BindAction(EKeyAction, ETriggerEvent::Triggered, this, &AMainCharacter::EKeyPressed);
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Triggered, this, &AMainCharacter::Attack);
	}
}

bool AMainCharacter::IsUnoccupied()
{
	return ActionState == EActionState::EAS_Unoccupied;
}

void AMainCharacter::GetHit(const FVector& HitLocation, AActor* Hitter)
{
	Super::GetHit(HitLocation, Hitter);

	if (IsAlive()) ActionState = EActionState::EAS_Hit;
}

void AMainCharacter::SetOverlappingItem(AItem* Item)
{
	OverlappingItem = Item;
}

void AMainCharacter::AddSouls(ASoul* Soul)
{	
	if (Attributes && HUDOverlay)
	{
		Attributes->AddSouls(Soul->GetSoulValue());
		HUDOverlay->SetTextSouls(Attributes->GetSouls());
	}
}

void AMainCharacter::AddGold(ATreasure* Treasure)
{
	if (Attributes && HUDOverlay)
	{
		Attributes->AddGold(Treasure->GetGoldValue());
		HUDOverlay->SetTextCoins(Attributes->GetGold());
	}
}

void AMainCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeMainCharacter();

	Tags.Add(FName("MainCharacter"));
}

bool AMainCharacter::CanAttack()
{
	return ActionState == EActionState::EAS_Unoccupied &&
		CharacterState != ECharacterState::ECS_Unequipped;
}

void AMainCharacter::Attack()
{
	Super::Attack();

	if (CanAttack())
	{
		ActionState = EActionState::EAS_Attacking;

		PlayAttackMontage();
	}
}

void AMainCharacter::AttackEnd()
{
	Super::AttackEnd();

	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::Death()
{
	Super::Death();

	ActionState = EActionState::EAS_Dead;
	bIsCameraLocked = true;

	DisableMeshCollision();
}

void AMainCharacter::AttachWeaponToHand()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToSocket(GetMesh(), FName("RightHandSocket"));
	}
}

void AMainCharacter::FinnishedAttachingToHand()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::AttachWeaponToBack()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->AttachMeshToSocket(GetMesh(), FName("SpineSocket"));
	}
}

void AMainCharacter::HitReactionEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
}

void AMainCharacter::DodgeEnd()
{
	ActionState = EActionState::EAS_Unoccupied;
	SetMovementModeToWalking();
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

void AMainCharacter::InitializeMainCharacter()
{
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		AddMainMappingContext(PlayerController);

		InitializeHUDOverlay(PlayerController);
	}
}

void AMainCharacter::AddMainMappingContext(APlayerController* PlayerController)
{
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(MappingContext, 0);
	}
}

void AMainCharacter::InitializeHUDOverlay(APlayerController* PlayerController)
{
	if (AGameHUD* GameHUD = Cast<AGameHUD>(PlayerController->GetHUD()))
	{
		HUDOverlay = GameHUD->GetHUDOverlay();
		if (HUDOverlay && Attributes)
		{
			HUDOverlay->SetHealthPercent(Attributes->GetHealthPercent());
			HUDOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
			HUDOverlay->SetTextCoins(0);
			HUDOverlay->SetTextSouls(0);
		}
	}
}

void AMainCharacter::SetHUDHealth()
{
	if (HUDOverlay && Attributes)
	{
		HUDOverlay->SetHealthPercent(Attributes->GetHealthPercent());
	}
}

void AMainCharacter::SetHUDStamina()
{
	if (HUDOverlay && Attributes)
	{
		HUDOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
	}
}

void AMainCharacter::Look(const FInputActionValue& Value)
{
	if (bIsCameraLocked) return;

	const FVector2D LookVector = Value.Get<FVector2D>();

	AddControllerPitchInput(LookVector.Y);
	AddControllerYawInput(LookVector.X);
}

void AMainCharacter::Dodge()
{
	if (!IsUnoccupied() || !HasEnoughStamina()) return;

	ActionState = EActionState::EAS_Dodging;

	Attributes->UseStamina(Attributes->DodgeCost);
	SetMovementModeToFlying();
	PlayDodgeAnimMontage();	
	SetHUDStamina();
}

void AMainCharacter::SetMovementModeToFlying()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Flying);
}

void AMainCharacter::SetMovementModeToWalking()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AMainCharacter::PlayDodgeAnimMontage()
{
	if (DodgeMontage)
	{
		PlayAnimMontage(DodgeMontage);
	}
}

void AMainCharacter::EKeyPressed()
{
	if (AWeapon* OverlappingWeapon = Cast<AWeapon>(OverlappingItem))
	{
		PickUpWeapon(OverlappingWeapon);
	}

	else if (CurrentWeapon)
	{
		EquipUnequip();
	}
}

bool AMainCharacter::HasEnoughStamina()
{
	return Attributes && Attributes->Stamina > Attributes->DodgeCost;
}

bool AMainCharacter::IsStaminaBarFull()
{
	return Attributes && Attributes->GetStaminaPercent() == 1.f;
}

void AMainCharacter::RegenStamina(float DeltaTime)
{
	if (!IsStaminaBarFull() && HUDOverlay)
	{
		Attributes->RegenStamina(DeltaTime);
		HUDOverlay->SetStaminaPercent(Attributes->GetStaminaPercent());
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

void AMainCharacter::PickUpWeapon(AWeapon* OverlappingWeapon)
{
	OverlappingWeapon->Equip(GetMesh(), FName("RightHandSocket"), this, this);
	CharacterState = ECharacterState::ECS_EquippedSword;
	OverlappingItem = nullptr;
	CurrentWeapon = OverlappingWeapon;
}

void AMainCharacter::EquipUnequip()
{
	if (CanEquip())
	{
		Equip();
	}
	else if (CanUnequip())
	{
		Unequip();
	}
}

void AMainCharacter::Equip()
{
	PlayEquipUnequipMontage(FName("Equip"));
	CharacterState = ECharacterState::ECS_EquippedSword;
	ActionState = EActionState::EAS_EquippingWeapon;
}

void AMainCharacter::Unequip()
{
	PlayEquipUnequipMontage(FName("Unequip"));
	CharacterState = ECharacterState::ECS_Unequipped;
	ActionState = EActionState::EAS_EquippingWeapon;
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