// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillRangeActor.h"

#include "Components/CapsuleComponent.h"
#include "RED/BattleField/Component/SkillComponent.h"

// Sets default values
ASkillRangeActor::ASkillRangeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Capsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	Capsule->SetupAttachment(Root);

	Capsule->SetCapsuleHalfHeight(1000.f);
	Capsule->SetCapsuleRadius(250.f);
	Capsule->SetGenerateOverlapEvents(true);
	Capsule->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	Capsule->SetCollisionProfileName(TEXT("SkillCollision"));

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CylinderMesh"));
	// Cylinder
	//StaticMesh->SetWorldScale3D(FVector{ 5,5,5 });
	// GizmoFullCircleHandle
	StaticMesh->SetWorldScale3D(FVector{ 2.25,2.25,2.25 });
	StaticMesh->SetWorldRotation(FRotator{ 90,0,0 });
	StaticMesh->SetupAttachment(Capsule);
	StaticMesh->SetGenerateOverlapEvents(false);
	StaticMesh->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;
	StaticMesh->SetCollisionProfileName(TEXT("NoCollision"));
	StaticMesh->SetHiddenInSceneCapture(true);
	StaticMesh->SetCastShadow(false);
	// /Engine/InteractiveToolsFramework/Meshes/GizmoFullCircleHandle.GizmoFullCircleHandle
	// /Engine/BasicShapes/Cylinder.Cylinder

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/InteractiveToolsFramework/Meshes/GizmoFullCircleHandle.GizmoFullCircleHandle"));

	if (MeshAsset.Succeeded())
	{
		StaticMesh->SetStaticMesh(MeshAsset.Object);
	}

	// /Engine/EngineDebugMaterials/VertexColorViewMode_RedOnly.VertexColorViewMode_RedOnly
	// /DatasmithContent/Materials/StdTranslucent/M_StdTranslucent.M_StdTranslucent
	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Game/Blueprints/BattleField/Character/Minion/RedMat.RedMat"));

	if (MaterialAsset.Succeeded())
	{
		StaticMesh->SetMaterial(0, MaterialAsset.Object);
	}

	bReplicates = true;
}

// Called when the game starts or when spawned
void ASkillRangeActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ASkillRangeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}