#pragma once

#include "CoreMinimal.h"
#include "Items/Item.h"
#include "Weapon.generated.h"

class USoundBase;
class UBoxComponent;
class USceneComponent;
class UFieldSystemComponent;
class URadialFalloff;
class URadialVector;
class UFieldSystemMetaDataFilter;

UCLASS()
class BOUNDLESSREALMS_API AWeapon : public AItem
{
	GENERATED_BODY()

public:

	AWeapon();

	FORCEINLINE UBoxComponent* GetWeaponBox() const { return WeaponBox; }

	// Equipping
	void Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator);
	void AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName);

	TArray<AActor*> IgnoreActors;

	bool bCanTrace = true;

protected:
	
	// <AActor>
	virtual void BeginPlay() override;
	// </AActor>

	UFUNCTION()
	void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:

	// Equipping
	void PlayFirstEquipSound();
	void DisableSphereCollision();
	void DisableBoxCollision();
	void DeactivateGlow();

	// On Box Overlap
	bool SameActorTypeHit(AActor* ThisActor,AActor* OtherActor);
	void BoxTrace(FHitResult& HitResult);
	void AddUniqueActorsToIgnore(TArray<AActor*, FDefaultAllocator>& ActorsToIgnore);
	void Hit(FHitResult& HitResult);
	void ApplyDamage(FHitResult& HitResult);
	void GetHit(FHitResult& HitResult);

	// Force Field
	void CreateField(const FVector& FieldLocation);
	void FieldRadialFalloff(URadialFalloff* RadialFalloff, const FVector& FieldLocation);
	void FieldRadialVector(URadialVector* RadialVector, const FVector& FieldLocation);
	void FieldMetaDataFilter(UFieldSystemMetaDataFilter* MetaDataFilter);
	void ApplyForceField(URadialFalloff* RadialFalloff, UFieldSystemMetaDataFilter* MetaDataFilter, URadialVector* RadialVector);

	
	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	USoundBase* FirstEquipSound; 

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UBoxComponent* WeaponBox;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceStart;

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	USceneComponent* BoxTraceEnd;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	FVector BoxTraceExtent = FVector(5.f);

	UPROPERTY(VisibleAnywhere, Category = "Weapon Properties")
	UFieldSystemComponent* FieldSystem;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	float Damage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Weapon Properties")
	bool bShowBoxDebug = false;
};
