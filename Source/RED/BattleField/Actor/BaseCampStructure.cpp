// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCampStructure.h"

#include "Components/CapsuleComponent.h"

#include "BaseCamp.h"
#include "RED/BattleField/BattleFieldGameMode.h"

ABaseCampStructure::ABaseCampStructure()
{
	BaseCampMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseCampMesh"));
	BaseCampMesh->SetupAttachment(RootComponent);

	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComponent"));
	CapsuleCollision->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	CapsuleCollision->SetupAttachment(BaseCampMesh);

	bReplicates = true;

	MiniMapIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiniMapIcon"));
	MiniMapIcon->AttachToComponent(
		BaseCampMesh,
		FAttachmentTransformRules::KeepRelativeTransform);
	MiniMapIcon->SetUsingAbsoluteRotation(true);

	MiniMapIcon->SetRelativeScale3D(FVector{ 10,10,10 });
	MiniMapIcon->SetCollisionProfileName(TEXT("NoCollision"));
	MiniMapIcon->SetVisibleInSceneCaptureOnly(true);
	MiniMapIcon->SetCastShadow(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/ControlRig/Controls/ControlRig_Triangle_solid.ControlRig_Triangle_solid"));

	if (MeshAsset.Succeeded())
	{
		MiniMapIcon->SetStaticMesh(MeshAsset.Object);
	}
}

void ABaseCampStructure::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &ABaseCampStructure::ReceiveDamage);
	}

	MiniMapIcon->SetRelativeLocation(FVector{ 0,0,300 });
	MiniMapIcon->SetWorldRotation(FRotator{ 0,0,0 });
}

float ABaseCampStructure::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	Health -= Damage;
	if (Health <= 0)
	{
		if(BaseCamp)
			BaseCamp->Destroy();
		ABattleFieldGameMode* BattleFieldGameMode = Cast<ABattleFieldGameMode>(GetWorld()->GetAuthGameMode());
		if(BattleFieldGameMode)
			BattleFieldGameMode->UpdateTeamScore(Team, -1);
		Destroy();
	}
	return Damage;
}

void ABaseCampStructure::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{

}