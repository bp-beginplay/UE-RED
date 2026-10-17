// Fill out your copyright notice in the Description page of Project Settings.


#include "PickOverlay.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

#include "PlayerPickWidget.h"
#include "RED/Lobby/LobbyPlayerState.h"

void UPickOverlay::SetPlayerPickWidgetArray()
{
	for (auto& iter : PlayerPickWidgetArray)
	{
		iter->SetCharacterNameText();
	}
}

void UPickOverlay::SetCharacterButtonEnable(bool _Enable)
{
	CheonIJinButton->SetIsEnabled(_Enable);
	Test3Button->SetIsEnabled(_Enable);
}

void UPickOverlay::SetCountDownText(FString _CountDown)
{
	CountDown->SetText(FText::FromString(_CountDown));
}

bool UPickOverlay::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	PlayerPickWidgetArray.Add(RedPlayerPickWidget1);
	PlayerPickWidgetArray.Add(BluePlayerPickWidget1);
	PlayerPickWidgetArray.Add(RedPlayerPickWidget2);
	PlayerPickWidgetArray.Add(BluePlayerPickWidget2);
	//PlayerPickWidgetArray.Add(RedPlayerPickWidget3);
	//PlayerPickWidgetArray.Add(BluePlayerPickWidget3);

	if (CheonIJinButton)
		CheonIJinButton->OnClicked.AddDynamic(this, &ThisClass::CheonIJinButtonClicked);

	if (Test3Button)
		Test3Button->OnClicked.AddDynamic(this, &ThisClass::Test3ButtonClicked);

	return true;
}

void UPickOverlay::CheonIJinButtonClicked_Implementation()
{
	ALobbyPlayerState* LobbyPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ALobbyPlayerState>();
	LobbyPlayerState->Server_SetCharacterName(TEXT("CheonIJin"));
}

void UPickOverlay::Test2ButtonClicked_Implementation()
{
	ALobbyPlayerState* LobbyPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ALobbyPlayerState>();
	LobbyPlayerState->Server_SetCharacterName(TEXT("Temp"));
}

void UPickOverlay::Test3ButtonClicked_Implementation()
{
	ALobbyPlayerState* LobbyPlayerState = GetWorld()->GetFirstPlayerController()->GetPlayerState<ALobbyPlayerState>();
	LobbyPlayerState->Server_SetCharacterName(TEXT("Yaco"));
}