#include "Items/Weapons/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Interfaces/HitInterface.h"
#include "MainCharacter.h"
#include "Field/FieldSystemComponent.h"
#include "Field/FieldSystemObjects.h"
#include "Field/FieldSystemActor.h"
#include "Field/FieldSystemTypes.h"
#include "NiagaraComponent.h"


AWeapon::AWeapon()
{
	WeaponBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Weapon Box"));
	WeaponBox->SetupAttachment(GetRootComponent());
	WeaponBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	WeaponBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);
	WeaponBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);

	BoxTraceStart = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace Start"));
	BoxTraceStart->SetupAttachment(GetRootComponent());

	BoxTraceEnd = CreateDefaultSubobject<USceneComponent>(TEXT("Box Trace End"));
	BoxTraceEnd->SetupAttachment(GetRootComponent());

	FieldSystem = CreateDefaultSubobject<UFieldSystemComponent>(TEXT("Field System"));
	FieldSystem->SetupAttachment(GetRootComponent());
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (!FieldSystem)
	{
		FieldSystem = NewObject<UFieldSystemComponent>(this, UFieldSystemComponent::StaticClass());
		FieldSystem->RegisterComponent();
		FieldSystem->AttachToComponent(GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	}

	WeaponBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnBoxOverlap);
}

void AWeapon::Equip(USceneComponent* InParent, FName InSocketName, AActor* NewOwner, APawn* NewInstigator)
{
	ItemState = EItemState::EIS_Equipped;

	SetOwner(NewOwner);
	SetInstigator(NewInstigator);

	AttachMeshToSocket(InParent, InSocketName);
	PlayFirstEquipSound();
	DisableSphereCollision();
	DisableBoxCollision();
	DeactivateGlow();
}

void AWeapon::AttachMeshToSocket(USceneComponent* InParent, const FName& InSocketName)
{
	FAttachmentTransformRules TransformRules(EAttachmentRule::SnapToTarget, true);
	ItemMesh->AttachToComponent(InParent, TransformRules, InSocketName);
}

void AWeapon::PlayFirstEquipSound()
{
	if (FirstEquipSound && GetOwner()->ActorHasTag("MainCharacter"))
	{
		UGameplayStatics::PlaySoundAtLocation(this, FirstEquipSound, GetActorLocation());
	}
}

void AWeapon::DisableSphereCollision()
{
	if (Sphere)
	{
		Sphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DisableBoxCollision()
{
	if (WeaponBox)
	{
		GetWeaponBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void AWeapon::DeactivateGlow()
{
	if (ItemGlow)
	{
		ItemGlow->Deactivate();
	}
}

void AWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnSphereOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);
}

void AWeapon::OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const FVector WeaponStart = BoxTraceStart->GetComponentLocation();
	const FVector WeaponEnd = BoxTraceEnd->GetComponentLocation();

	TArray<AActor*> ActorsToIgnore;
	FHitResult HitResult;

	AddUniqueActorsToIgnore(ActorsToIgnore);
	BoxTrace(WeaponStart, WeaponEnd, ActorsToIgnore, HitResult);
	Hit(HitResult);
}

void AWeapon::AddUniqueActorsToIgnore(TArray<AActor*, FDefaultAllocator>& ActorsToIgnore)
{
	for (AActor* Actor : IgnoreActors)
	{
		ActorsToIgnore.AddUnique(Actor);
	}
}

void AWeapon::BoxTrace(const FVector& WeaponStart, const FVector& WeaponEnd, TArray<AActor*, FDefaultAllocator>& ActorsToIgnore, FHitResult& HitResult)
{
	UKismetSystemLibrary::BoxTraceSingle(
		this,
		WeaponStart,
		WeaponEnd,
		FVector(5.f, 5.f, 5.f),
		BoxTraceStart->GetComponentRotation(),
		ETraceTypeQuery::TraceTypeQuery1,
		false,
		ActorsToIgnore,
		EDrawDebugTrace::None,
		HitResult,
		true);
}

void AWeapon::Hit(FHitResult& HitResult)
{
	if (HitResult.GetActor())
	{
		ApplyDamage(HitResult);
		GetHit(HitResult);
		IgnoreActors.AddUnique(HitResult.GetActor());
		CreateField(HitResult.ImpactPoint);
	}
}

void AWeapon::ApplyDamage(FHitResult& HitResult)
{
	UGameplayStatics::ApplyDamage(
		HitResult.GetActor(),
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass());
}

void AWeapon::GetHit(FHitResult& HitResult)
{
	if (IHitInterface* HitInterface = Cast<IHitInterface>(HitResult.GetActor()))
	{
		HitInterface->GetHit(HitResult.ImpactPoint);
	}
}

void AWeapon::CreateField(const FVector& FieldLocation)
{
	URadialFalloff* RadialFalloff = NewObject<URadialFalloff>(this);
	FieldRadialFalloff(RadialFalloff, FieldLocation);

	URadialVector* RadialVector = NewObject<URadialVector>(this);
	FieldRadialVector(RadialVector, FieldLocation);

	UFieldSystemMetaDataFilter* MetaDataFilter = NewObject<UFieldSystemMetaDataFilter>(this);
	FieldMetaDataFilter(MetaDataFilter);

	ApplyForceField(RadialFalloff, MetaDataFilter, RadialVector);
}

void AWeapon::FieldRadialFalloff(URadialFalloff* RadialFalloff, const FVector& FieldLocation)
{
	RadialFalloff->Magnitude = 1000000000.0f;
	RadialFalloff->Radius = 200.0f;
	RadialFalloff->Position = FieldLocation;
	RadialFalloff->Falloff = EFieldFalloffType::Field_FallOff_None;
}

void AWeapon::FieldRadialVector(URadialVector* RadialVector, const FVector& FieldLocation)
{
	RadialVector->Magnitude = 5000000000.0f;
	RadialVector->Position = FieldLocation;
}

void AWeapon::FieldMetaDataFilter(UFieldSystemMetaDataFilter* MetaDataFilter)
{
	MetaDataFilter->SetMetaDataFilterType(EFieldFilterType::Field_Filter_Dynamic,
		EFieldObjectType::Field_Object_Destruction,
		EFieldPositionType::Field_Position_CenterOfMass);
}

void AWeapon::ApplyForceField(URadialFalloff* RadialFalloff, UFieldSystemMetaDataFilter* MetaDataFilter, URadialVector* RadialVector)
{
	FieldSystem->ApplyPhysicsField(true,
		EFieldPhysicsType::Field_ExternalClusterStrain,
		nullptr,
		RadialFalloff);

	FieldSystem->ApplyPhysicsField(true,
		EFieldPhysicsType::Field_LinearForce,
		MetaDataFilter,
		RadialVector);
}