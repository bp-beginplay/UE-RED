// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameState.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "LobbyGameMode.h"
#include "LobbyHUD.h"
#include "LobbyPlayerController.h"
#include "LobbyPlayerState.h"

void ALobbyGameState::AddPlayerStateArray(ALobbyPlayerState* _NewPlayerName)
{
	PlayerStateArray.Add(_NewPlayerName);

	// 플레이어가 들어오면 호스트 JoinPlayer유저위젯 설정
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	LobbyPlayerController->SetHUDTeamOverlay();
}

bool ALobbyGameState::AddReadyPlayer()
{
	++ReadyPlayer;

	ALobbyGameMode* LobbyGameMode = Cast<ALobbyGameMode>(GetWorld()->GetAuthGameMode());
	int Player = 0;
	if (LobbyGameMode)
		Player = LobbyGameMode->NumPlayers;

	if (ReadyPlayer == Player)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ALobbyGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyGameState, PlayerStateArray);
}

void ALobbyGameState::OnRep_PlayerStateArray()
{
	// 플레이어가 들어오면 클라이언트 JoinPlayer유저위젯 설정
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	LobbyPlayerController->SetHUDTeamOverlay();
}