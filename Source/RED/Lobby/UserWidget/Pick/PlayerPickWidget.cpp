// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerPickWidget.h"

#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "RED/Lobby/LobbyPlayerState.h"
#include "RED/Info/CharacterListRow.h"

void UPlayerPickWidget::SetPlayerState(ALobbyPlayerState* _PlayerState)
{
	LobbyPlayerState = _PlayerState;
	SetPlayerNameText();
	LobbyPlayerState->SetPickSequence(PickSequence);
}

void UPlayerPickWidget::SetCharacterNameText()
{
	if (LobbyPlayerState)
	{
		FCharacterListRow* CharacterListRow = CharacterDataTable->FindRow<FCharacterListRow>(LobbyPlayerState->GetCharacterName(), TEXT(""));
		
		if (CharacterListRow)
		{
			FSlateBrush CharacterBrush;
			CharacterBrush.SetResourceObject(CharacterListRow->CharacterImage);
			CharacterName->SetText(FText::FromName(LobbyPlayerState->GetCharacterName()));
			CharacterImage->SetBrush(CharacterBrush);
		}

	}
}

void UPlayerPickWidget::SetPlayerNameText()
{
	if (LobbyPlayerState)
	{
		PlayerName->SetText(FText::FromString(LobbyPlayerState->GetPlayerName()));
	}
}