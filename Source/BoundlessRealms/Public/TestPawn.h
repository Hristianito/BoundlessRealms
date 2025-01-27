#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TestPawn.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;

UCLASS()
class BOUNDLESSREALMS_API ATestPawn : public APawn
{
	GENERATED_BODY()

public:

	ATestPawn();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:

	void MoveForward(float Value);

private:

	UPROPERTY(EditAnywhere)
	UCapsuleComponent* Capsule;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* PawnMesh;
};
