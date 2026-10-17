// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamButton.h"

#include "Components/Button.h"

#include "RED/Lobby/LobbyPlayerState.h"

bool UTeamButton::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (TeamButton)
		TeamButton->OnClicked.AddDynamic(this, &ThisClass::TeamButtonClicked);

	return true;
}

void UTeamButton::TeamButtonClicked()
{
	ALobbyPlayerState* LobbyPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ALobbyPlayerState>();

	if (LobbyPlayerState->GetTeam() != Team)
	{
		LobbyPlayerState->Server_SetTeam(Team);
	}
	else
	{
		LobbyPlayerState->Server_SetTeam(ETeam::ET_NoTeam);
	}
}
