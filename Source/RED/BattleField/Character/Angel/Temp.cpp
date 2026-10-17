// Fill out your copyright notice in the Description page of Project Settings.


#include "Temp.h"

#include "RED/BattleField/BattleFieldGameState.h"
#include "RED/BattleField/BattleFieldPlayerState.h"

ATemp::ATemp()
{
}

void ATemp::BeginPlay()
{
	Super::BeginPlay();

	CachedGameState = GetWorld()->GetGameState<ABattleFieldGameState>();
}

void ATemp::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ATemp::Heal()
{
	if (BattleFieldPlayerState)
	{
		float Health = BattleFieldPlayerState->Health;
		float MaxHealth = BattleFieldPlayerState->MaxHealth;
		float HealAmount = MaxHealth * 0.001f;

		BattleFieldPlayerState->CalculateHealth(HealAmount * -1);

		TArray<ABattleFieldPlayerState*> CloseAllyPlayerList;

		if (Health + HealAmount >= MaxHealth)
		{
			SavedHealAmount = (Health + HealAmount) - MaxHealth;

			for (APlayerState* PS : CachedGameState->PlayerArray)
			{
				if (PS == GetPlayerState())
				{
					continue;
				}

				if (IsEnemy(PS) || !PS)
				{
					continue;
				}

				APlayerCharacter* AllyPlayer = Cast<APlayerCharacter>(PS->GetPawn());

				if (!AllyPlayer)
				{
					continue;
				}

				float DistSq = FVector::DistSquared(GetActorLocation(), AllyPlayer->GetActorLocation());

				if (DistSq <= PassiveRange * PassiveRange)
				{
					ABattleFieldPlayerState* BPS = Cast<ABattleFieldPlayerState>(PS);
					CloseAllyPlayerList.Add(BPS);
				}
			}

			ABattleFieldPlayerState* tempAlly = nullptr;
			float HPPercent = 100.f;
			for (auto AllyPlayer : CloseAllyPlayerList)
			{
				float tempHPPercent = AllyPlayer->Health / AllyPlayer->MaxHealth;
				if (tempHPPercent <= HPPercent)
				{
					HPPercent = tempHPPercent;
					tempAlly = AllyPlayer;
				}
			}

			if (tempAlly)
			{
				tempAlly->CalculateHealth(SavedHealAmount * -1);
			}

			Health = MaxHealth;
		}
	}
}