// Fill out your copyright notice in the Description page of Project Settings.

#include "TestPawn.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

ATestPawn::ATestPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule"));
	SetRootComponent(Capsule);

	PawnMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	PawnMesh->SetupAttachment(GetRootComponent());

	AutoPossessPlayer = EAutoReceiveInput::Player0;
}

void ATestPawn::BeginPlay()
{
	Super::BeginPlay();
	
}

void ATestPawn::MoveForward(float Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Value: %f"), Value)
}

void ATestPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ATestPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &ATestPawn::MoveForward);
}

