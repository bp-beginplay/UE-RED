// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "Kismet/GameplayStatics.h"

#include "LobbyGameState.h"
#include "LobbyPlayerController.h"
#include "LobbyPlayerState.h"

#include "RED/Info/CharacterListRow.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/BattleFieldGameMode.h"

ALobbyGameMode::ALobbyGameMode()
{
	bUseSeamlessTravel = true;
}

void ALobbyGameMode::ServerTravel()
{
	UWorld* World = GetWorld();

	if (World)
	{
		World->ServerTravel(FString("/Game/Maps/BattleField?seamless"));
	}
}

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerCount++;

	ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(this));
	ALobbyPlayerState* LobbyPlayerState = NewPlayer->GetPlayerState<ALobbyPlayerState>();
	LobbyGameState->AddPlayerStateArray(LobbyPlayerState);
}

void ALobbyGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsActivePickOverlay)
	{
		if (GetWorld()->GetTimeSeconds() - PickStartTime >= PickCountDown)
		{
			PickStartTime = GetWorld()->GetTimeSeconds();
			ActiveOnce = true;
		}

		if (ActiveOnce)
		{
			ActiveOnce = false;
			if (PickSequence == PlayerCount + 1)
			{
				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(*It);
					LobbyPlayerController->SetIsActivePickOverlay(false);
				}
				IsActivePickOverlay = false;
				ServerTravel();
			}
			else
			{
				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(*It);
					ALobbyPlayerState* LobbyPlayerState = LobbyPlayerController->GetPlayerState<ALobbyPlayerState>();

					if (LobbyPlayerState->GetPickSequence() == PickSequence - 1 && LobbyPlayerState->GetCharacterName() == FName("None"))
					{
						TArray<FName>CharacterNameArray = CharacterDataTable->GetRowNames();
						int RandomIndex = FMath::RandRange(0, CharacterDataTable->GetRowNames().Num() - 1);
						LobbyPlayerState->Server_SetCharacterName(CharacterNameArray[RandomIndex]);
					}

					if (LobbyPlayerState->GetPickSequence() == PickSequence)
					{
						LobbyPlayerController->Client_SetCanPickCharacter(true);
					}
					else
					{
						LobbyPlayerController->Client_SetCanPickCharacter(false);
					}
				}
				PickSequence++;
			}
		}
	}
}

void ALobbyGameMode::BeginPlay()
{
	Super::BeginPlay();

	//bUseSeamlessTravel = true;
}

void ALobbyGameMode::SetActivePickOverlay(bool _IsActive)
{
	PickStartTime = GetWorld()->GetTimeSeconds();
	IsActivePickOverlay = _IsActive;
}
