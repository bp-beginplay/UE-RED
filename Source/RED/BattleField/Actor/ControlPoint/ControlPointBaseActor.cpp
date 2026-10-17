// Fill out your copyright notice in the Description page of Project Settings.


#include "ControlPointBaseActor.h"

#include "Net/UnrealNetwork.h"
#include "RED/RED.h"
#include "RED/BattleField/Character/Minion/MinionSpawnSlotBaseActor.h"

// Sets default values
AControlPointBaseActor::AControlPointBaseActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ControlPointMesh = CreateDefaultSubobject<UStaticMeshComponent>("ControlPointMesh");
	ControlPointMesh->SetupAttachment(GetRootComponent());
}

// Called when the game starts or when spawned
void AControlPointBaseActor::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AControlPointBaseActor::Server_CheckOverlaps);
}

// Called every frame
void AControlPointBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AControlPointBaseActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void AControlPointBaseActor::SetMinionMovePoint(ETeam _Team, EMinionSpawnSlotIndex _index, int32 _Value)
{
	if (Team == _Team)
	{
		for (auto MinionSpawnSlot : OwnedMinionSpawnSlotList)
		{
			if (MinionSpawnSlot->GetSlotIndex() == _index)
			{
				MinionSpawnSlot->SetMinionTarget(_Value);
			}
		}
	}
}

void AControlPointBaseActor::Server_CheckOverlaps_Implementation()
{
	TArray<AActor*> Overlaps;
	ControlPointMesh->GetOverlappingActors(
		Overlaps,
		AMinionSpawnSlotBaseActor::StaticClass()
	);

	for (auto Actor : Overlaps)
	{
		if (AMinionSpawnSlotBaseActor* SpawnSlot = Cast<AMinionSpawnSlotBaseActor>(Actor))
		{
			OwnedMinionSpawnSlotList.Add(SpawnSlot);
			SpawnSlot->SetOwnerControlPoint(this);
		}
	}

	if (OwnedMinionSpawnSlotList.Num() == 0)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AControlPointBaseActor::Server_CheckOverlaps);
	}
}