// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerState.h"

#include "Net/UnrealNetwork.h"

#include "LobbyPlayerController.h"
#include "RED/Red_GameInstance.h"
#include "RED/BattleField/BattleFieldPlayerState.h"

void ALobbyPlayerState::Server_SetTeam_Implementation(ETeam _Team)
{
	Team = _Team;
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	LobbyPlayerController->SetHUDTeamOverlay();
}

void ALobbyPlayerState::Server_SetCharacterName_Implementation(FName _Name)
{
	PickCharacter = _Name;
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	LobbyPlayerController->SetHUDPickOverlay();

	Multi_SetPickCharacter(GetUniqueId(), PickCharacter);
	
}

void ALobbyPlayerState::Multi_SetPickCharacter_Implementation(const FUniqueNetIdRepl& _UniqueNetID, FName _Name)
{
	Cast<URed_GameInstance>(GetGameInstance())->SetPickCharacter(_UniqueNetID, _Name);
}

void ALobbyPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALobbyPlayerState, Team);
	DOREPLIFETIME(ALobbyPlayerState, PickCharacter);
}

void ALobbyPlayerState::OnRep_SetTeam()
{
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	LobbyPlayerController->SetHUDTeamOverlay();
}

void ALobbyPlayerState::OnRep_SetCharacterName()
{
	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	LobbyPlayerController->SetHUDPickOverlay();
}

void ALobbyPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ABattleFieldPlayerState* NewPlayerState = Cast<ABattleFieldPlayerState>(PlayerState);
	if (NewPlayerState)
	{
		NewPlayerState->CharacterName = PickCharacter;
		NewPlayerState->Team = Team;
	}
}

void ALobbyPlayerState::SetMinionSlot(EMinionSpawnSlotIndex _SlotIndex)
{
	OwnedSlot = _SlotIndex;

	Cast<URed_GameInstance>(GetGameInstance())->AddMinionSlot(GetUniqueId(), OwnedSlot);
}