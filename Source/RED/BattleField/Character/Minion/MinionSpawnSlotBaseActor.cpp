// Fill out your copyright notice in the Description page of Project Settings.


#include "MinionSpawnSlotBaseActor.h"
#include "BaseMinionCharacter.h"
#include "MinionAIController.h"
#include "../../../Interface/ActorInterface.h"
#include "RED/RED.h"
#include "RED/BattleField/BattleFieldGameMode.h"
#include "RED/BattleField/Actor/ControlPoint/ControlPointBaseActor.h"

// Sets default values
AMinionSpawnSlotBaseActor::AMinionSpawnSlotBaseActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AMinionSpawnSlotBaseActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		StartMinionSpawn();
	}
}

void AMinionSpawnSlotBaseActor::Server_SpawnMinionCycle_Implementation()
{
	GetWorld()->GetTimerManager().SetTimer(
		ThirdSpawnTimerHandle,
		this,
		&AMinionSpawnSlotBaseActor::SpawnMinionTeam,
		0.75f,
		true
	);
}

void AMinionSpawnSlotBaseActor::SpawnMinionTeam()
{
	if (IActorInterface::Execute_GetETeam(OwnerControlPoint) == ETeam::ET_NoTeam)
	{
		return;
	}
	++SpawnCount;
	if (SpawnCount <= 1)
	{
		SpawnMinion(SpawnMeleeMinionClass);
	}
	else if (SpawnCount <= 2)
	{
		SpawnMinion(SpawnRangeMinionClass);
	}

	if (SpawnCount >= 2)
	{
		GetWorldTimerManager().ClearTimer(ThirdSpawnTimerHandle);
		SpawnCount = 0; // 다음 웨이브를 위해 초기화
	}
}

void AMinionSpawnSlotBaseActor::SpawnMinion(TSubclassOf<ABaseMinionCharacter> _Class)
{
	if (_Class)
	{
		FVector SpawnLocation = GetActorLocation();
		FRotator SpawnRotation = FRotator::ZeroRotator;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		ABaseMinionCharacter* Minion = GetWorld()->SpawnActor<ABaseMinionCharacter>(
			_Class,
			SpawnLocation,
			SpawnRotation,
			SpawnParams // 파라미터 추가
		);

		if (Minion)
		{
			OwnedMinionList.Add(Minion);
			Minion->OnDestroyed.AddDynamic(this, &AMinionSpawnSlotBaseActor::MinionOnDestroyed);
			Minion->SetTargetControlPointValue(ControlPointValue);
			Minion->Server_SetTeam(IActorInterface::Execute_GetETeam(OwnerControlPoint));
			Minion->Server_SetFOWDrawerTeam(IActorInterface::Execute_GetTeam(OwnerControlPoint));

			Minion->Mul_SetHPBarColor(IActorInterface::Execute_GetETeam(Minion));

			if (_Class == SpawnMeleeMinionClass)
			{
				Minion->SetMaxHP(50.f);
				Minion->SetHP(50.f);
				Minion->SetIsMelee(EMinionType::MELEE);
				Minion->Mul_NotifyHPChanged(Minion->GetHP(), 0, Minion->GetMaxHP());
				if(HasAuthority())
				{
					Minion->OnRep_MinionStats();
				}
			}
			else if (_Class == SpawnRangeMinionClass)
			{
				Minion->SetMaxHP(40.f);
				Minion->SetHP(40.f);
				Minion->SetIsMelee(EMinionType::RANGE);
				Minion->Mul_NotifyHPChanged(Minion->GetHP(), 0, Minion->GetMaxHP());
				if (HasAuthority())
				{
					Minion->OnRep_MinionStats();
				}
			}
		}
	}
}

void AMinionSpawnSlotBaseActor::MinionOnDestroyed(AActor* DestroyedActor)
{
	OwnedMinionList.Remove(Cast<ABaseMinionCharacter>(DestroyedActor));
}

// Called every frame
void AMinionSpawnSlotBaseActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AMinionSpawnSlotBaseActor::SetMinionTarget(int32 _Value)
{
	ControlPointValue = _Value;
	for (auto Minion : OwnedMinionList)
	{
		AMinionAIController* MinionController = Cast<AMinionAIController>(Minion->GetController());
		MinionController->FindNewTarget(_Value);
	}
}

void AMinionSpawnSlotBaseActor::StartMinionSpawn()
{
	ABattleFieldGameMode* BGM = Cast<ABattleFieldGameMode>(GetWorld()->GetAuthGameMode());
	if (BGM)
	{
		if (!BGM->GameStart)
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &AMinionSpawnSlotBaseActor::StartMinionSpawn);
			return;
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		FirstSpawnTimerHandle,
		this,
		&AMinionSpawnSlotBaseActor::Server_SpawnMinionCycle,
		30.0f,
		true,
		30.0f
	);
}