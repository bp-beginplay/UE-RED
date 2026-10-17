// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamOverlay.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"

#include "RED/Lobby/LobbyGameState.h"
#include "RED/Lobby/LobbyPlayerState.h"
#include "RED/Lobby/LobbyPlayerController.h"
#include "RED/Lobby/UserWidget/Team/TeamButton.h"

void UTeamOverlay::SetTeamPlayerTextArray()
{
	for (auto& iter : TeamPlayerTextArray)
	{
		iter->SetText(FText::FromString("None"));
	}

	ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(this));
	TArray<ALobbyPlayerState*> PlayerStateArray = LobbyGameState->GetPlayerStateArray();

	int i = 0;
	int j = 6;
	int k = 9;
	for (auto& iter : PlayerStateArray)
	{
		if (iter)
		{
			switch (iter->GetTeam())
			{
			case ETeam::ET_NoTeam:
				TeamPlayerTextArray[i]->SetText(FText::FromString(iter->GetPlayerName()));
				iter->SetMinionSlot(EMinionSpawnSlotIndex::NONE);
				i++;
				break;
			case ETeam::ET_RedTeam:
				TeamPlayerTextArray[j]->SetText(FText::FromString(iter->GetPlayerName()));
				switch (j)
				{
				case 6:
					iter->SetMinionSlot(EMinionSpawnSlotIndex::SLOT_01);
					break;
				case 7:
					iter->SetMinionSlot(EMinionSpawnSlotIndex::SLOT_02);
					break;
				case 8:
					iter->SetMinionSlot(EMinionSpawnSlotIndex::SLOT_03);
					break;
				default:
					break;
				}
				j++;
				break;
			case ETeam::ET_BlueTeam:
				TeamPlayerTextArray[k]->SetText(FText::FromString(iter->GetPlayerName()));
				switch (k)
				{
				case 9:
					iter->SetMinionSlot(EMinionSpawnSlotIndex::SLOT_01);
					break;
				case 10:
					iter->SetMinionSlot(EMinionSpawnSlotIndex::SLOT_02);
					break;
				case 11:
					iter->SetMinionSlot(EMinionSpawnSlotIndex::SLOT_03);
					break;
				default:
					break;
				}
				k++;
				break;
			default:
				break;
			}
		}
	}
}

bool UTeamOverlay::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	TeamPlayerTextArray.Add(NoTeamPlayer1);
	TeamPlayerTextArray.Add(NoTeamPlayer2);
	TeamPlayerTextArray.Add(NoTeamPlayer3);
	TeamPlayerTextArray.Add(NoTeamPlayer4);
	TeamPlayerTextArray.Add(NoTeamPlayer5);
	TeamPlayerTextArray.Add(NoTeamPlayer6);
	TeamPlayerTextArray.Add(RedTeamPlayer1);
	TeamPlayerTextArray.Add(RedTeamPlayer2);
	TeamPlayerTextArray.Add(RedTeamPlayer3);
	TeamPlayerTextArray.Add(BlueTeamPlayer1);
	TeamPlayerTextArray.Add(BlueTeamPlayer2);
	TeamPlayerTextArray.Add(BlueTeamPlayer3);

	if (ReadyButton)
		ReadyButton->OnClicked.AddDynamic(this, &ThisClass::ReadyButtonClicked);

	return true;
}

void UTeamOverlay::ReadyButtonClicked()
{
	ReadyButton->SetIsEnabled(false);

	ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(GetWorld()->GetFirstPlayerController());
	LobbyPlayerController->Server_AddReadyPlayerCount();
}