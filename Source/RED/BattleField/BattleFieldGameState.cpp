// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleFieldGameState.h"

#include "BattleFieldGameMode.h"
#include "Actor/ControlPoint/ControlPointBaseActor.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABattleFieldGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleFieldGameState, ControlPointsTeam);
}

void ABattleFieldGameState::SetControlPointTeam(int32 ControlPointIndex, ETeam Team)
{
	ControlPointsTeam[ControlPointIndex - 1] = Team;
	OnRep_Team();
}

void ABattleFieldGameState::OnRep_Team()
{
	OnTeamChanged.Broadcast();
}

void ABattleFieldGameState::InitGameState()
{
	ControlPointsTeam.Init(ETeam::ET_NoTeam, 7);
}
