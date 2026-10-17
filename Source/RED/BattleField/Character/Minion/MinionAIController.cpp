// Fill out your copyright notice in the Description page of Project Settings.

#include "MinionAIController.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Navigation/PathFollowingComponent.h"
#include "BaseMinionCharacter.h"
#include "Navigation/CrowdFollowingComponent.h"

#include "RED/BattleField/Actor/ControlPoint/ControlPointBaseActor.h"
#include "RED/BattleField/BattleFieldGameMode.h"
#include "RED/BattleField/BattleFieldPlayerState.h"

#include "RED/RED.h"

AMinionAIController::AMinionAIController()
{
}

void AMinionAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	OwnedMinion = Cast<ABaseMinionCharacter>(InPawn);

	ABattleFieldGameMode* GameMode =
		Cast<ABattleFieldGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	check(GameMode);

	ControlPointList = &GameMode->GetControlPointList();

	TargetControlPointValue = OwnedMinion->GetTargetControlPointValue();

	FindNewTarget(TargetControlPointValue);
}

void AMinionAIController::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(CheckDistHandle, this, &AMinionAIController::CalDistToTarget, 0.1f, true);

	AttackRangeSq = AttackRange * AttackRange;
}

void AMinionAIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AMinionAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// 모든 Target에 대해 델리게이트 제거
	for (AActor* TargetActor : TargetList)
	{
		if (IsValid(TargetActor))
		{
			TargetActor->OnDestroyed.RemoveDynamic(
				this,
				&AMinionAIController::OnDestroyed
			);
		}
	}

	TargetList.Empty();
}

void AMinionAIController::BeginOverlap(AActor* _Actor)
{
	if (!IsDied)
	{
		if (IsValid(_Actor))
		{
			if (IsEnemy(_Actor))
			{
				TargetList.AddUnique(_Actor);
				_Actor->OnDestroyed.AddUniqueDynamic(this, &AMinionAIController::OnDestroyed);
			}
			else
			{
				return;
			}

			// 기존 목표가 점령지가 아닌 엑터라면
			// 목표가 죽거나 시야나 범위 내에서 사라지기 전(리스트에서 제거되기 전)까지는 계속 타겟팅
			if (Cast<AControlPointBaseActor>(Target))
			{
				FindNewTarget(TargetControlPointValue);
			}
		}
	}
}

void AMinionAIController::EndOverlap(AActor* _Actor)
{
	if (!IsDied)
	{
		if (IsValid(_Actor))
		{
			TargetList.Remove(_Actor);
			TargetCharacter = nullptr;

			if (Target == _Actor)
			{
				FindNewTarget(TargetControlPointValue);
			}
		}
	}
}

void AMinionAIController::OnDestroyed(AActor* _Actor)
{
	if (IsValid(_Actor))
	{
		TargetList.Remove(_Actor);

		_Actor->OnDestroyed.RemoveDynamic(this, &AMinionAIController::OnDestroyed);

		if (Target == _Actor)
		{
			FindNewTarget(TargetControlPointValue);
		}
	}
}

bool AMinionAIController::IsEnemy(AActor* _OtherActor) const
{

	if (!IsValid(_OtherActor))
	{
		return false;
	}

	if (!_OtherActor->Implements<UActorInterface>())
	{
		return false;
	}

	if (Cast<AControlPointBaseActor>(_OtherActor))
	{
		return false;
	}

	const int32 OtherTeam = IActorInterface::Execute_GetTeam(_OtherActor);
	const int32 MyTeam = IActorInterface::Execute_GetTeam(OwnedMinion);

	if (MyTeam == 2 || OtherTeam == 2)
	{
		return false;
	}

	return OtherTeam != MyTeam;
}

void AMinionAIController::CalDistToTarget()
{
	if (Target != LastTarget || GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		MoveToActor(Target);
		LastTarget = Target;
	}

	if (TargetCharacter)
	{
		if (IActorInterface::Execute_GetVisibility(TargetCharacter) == 0)
		{
			IsAttacking = false;
			TargetCharacter = nullptr;
			ClearFocus(EAIFocusPriority::Gameplay);
			FindNewTarget(TargetControlPointValue);
		}
	}

	if (TargetCharacter)
	{
		float DistSq = TargetCharacter->GetSquaredDistanceTo(OwnedMinion);
		if (DistSq <= AttackRangeSq)
		{
			if (!IsAttacking)
			{
				if (GetMoveStatus() != EPathFollowingStatus::Idle)
				{
					StopMovement();
				}
				IsAttacking = true;
				SetFocus(TargetCharacter);
			}
		}
		else
		{
			if (IsAttacking)
			{
				IsAttacking = false;
				ClearFocus(EAIFocusPriority::Gameplay);
				MoveToActor(TargetCharacter);
			}
		}
	}
	else
	{
		IsAttacking = false;
		ClearFocus(EAIFocusPriority::Gameplay);
	}

	OwnedMinion->SetIsAttacking(IsAttacking);
}

void AMinionAIController::Controller_ApplyDamage(float Damage)
{
	UGameplayStatics::ApplyDamage(
		TargetCharacter,	// 데미지 받을 대상
		Damage,				// 데미지 수치
		this,				// Instigator (누가 때렸는지)
		this,				// DamageCauser (무기로 때렸으면 무기)
		UDamageType::StaticClass()
	);
}

void AMinionAIController::FindNewTarget(int32 _Value)
{
	TargetControlPointValue = _Value;

	if (!OwnedMinion)
	{
		return;
	}

	if (TargetList.IsEmpty())
	{
		SetTargetControlZone(TargetControlPointValue);
	}
	else
	{
		Target = FindClosestTarget();
	}

	if (!IsValid(TargetCharacter) || IActorInterface::Execute_GetIsDied(TargetCharacter))
	{
		//이러면 포탑은 지정 안되서 나중에 수정 필요
		TargetCharacter = Cast<ABaseCharacter>(Target);
	}
}

AActor* AMinionAIController::FindClosestTarget()
{
	if (!OwnedMinion)
	{
		return nullptr;
	}

	float tempDistance = 10000.f;
	AActor* tempActor = nullptr;
	for (auto target : TargetList)
	{
		if (IActorInterface::Execute_GetVisibility(target) == 0)
		{
			continue;
		}

		if (IActorInterface::Execute_GetIsDied(target))
		{
			continue;
		}

		float Distance = OwnedMinion->GetDistanceTo(target);

		if (Distance < tempDistance)
		{
			tempDistance = Distance;
			tempActor = target;
		}
	}

	return tempActor ? tempActor : SetTargetControlZone(TargetControlPointValue);
}

AActor* AMinionAIController::SetTargetControlZone(int32 _Value)
{
	for (auto ControlPoint : *ControlPointList)
	{
		if (ControlPoint->GetControlPointValue() == _Value)
		{
			Target = ControlPoint;
			break;
		}
	}

	return Target;
}

void AMinionAIController::Died()
{
	if (!IsDied)
	{
		IsDied = true;

		// 모든 Target에 대해 델리게이트 제거
		for (AActor* TargetActor : TargetList)
		{
			if (IsValid(TargetActor))
			{
				TargetActor->OnDestroyed.RemoveDynamic(
					this,
					&AMinionAIController::OnDestroyed
				);
			}
		}

		TargetList.Empty();

		StopMovement();
	}
}