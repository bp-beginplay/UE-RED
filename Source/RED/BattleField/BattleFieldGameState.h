// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "RED/Info/EnumInfo.h"
#include "BattleFieldGameState.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnTeamChanged);

UCLASS()
class RED_API ABattleFieldGameState : public AGameState
{
	GENERATED_BODY()

protected:
	UPROPERTY()
	TArray<class AControlPointBaseActor*> ControlPointList;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Team)
	TArray<ETeam> ControlPointsTeam;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:
	UFUNCTION()
	void SetControlPointTeam(int32 ControlPointIndex, ETeam Team);

	UFUNCTION()
	void OnRep_Team();

	UFUNCTION()
	void InitGameState();

	FOnTeamChanged OnTeamChanged;

	UFUNCTION()
	TArray<ETeam> GetControlPointsTeam() const { return ControlPointsTeam; }
};
