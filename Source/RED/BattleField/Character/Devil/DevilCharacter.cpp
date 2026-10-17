// Fill out your copyright notice in the Description page of Project Settings.


#include "DevilCharacter.h"

#include "RED/BattleField/BattleFieldPlayerState.h"

void ADevilCharacter::BeginPlay()
{
	Super::BeginPlay();

	DevilPassive();
}

void ADevilCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADevilCharacter::DevilPassive()
{
	if (!DevilPassiveInit)
	{
		ABattleFieldPlayerState* BPS = GetPlayerState<ABattleFieldPlayerState>();
		if (BPS)
		{
			BPS->BaseStats.LifeSteel += 0.1f;
			BPS->CalculateBuff();

			DevilPassiveInit = true;
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ADevilCharacter::DevilPassive);
		}
	}
}
