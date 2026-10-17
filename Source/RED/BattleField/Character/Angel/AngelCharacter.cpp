// Fill out your copyright notice in the Description page of Project Settings.


#include "AngelCharacter.h"

#include "RED/BattleField/BattleFieldPlayerState.h"

void AAngelCharacter::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		AngelPassiveHandle,
		this,
		&AAngelCharacter::Heal,
		0.1f,
		true
	);
}

void AAngelCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAngelCharacter::Heal()
{
	if (BattleFieldPlayerState)
	{
		float Health = BattleFieldPlayerState->Health;
		float MaxHealth = BattleFieldPlayerState->MaxHealth;
		if (Health < MaxHealth)
		{
			BattleFieldPlayerState->CalculateHealth(MaxHealth * 0.01f * -1);
		}
	}
}
