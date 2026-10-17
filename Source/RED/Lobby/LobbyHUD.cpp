// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyHUD.h"

#include "Kismet/GameplayStatics.h"

#include "LobbyGameState.h"
#include "LobbyPlayerState.h"
#include "UserWidget/Pick/PickOverlay.h"
#include "UserWidget/Pick/PlayerPickWidget.h"
#include "UserWidget/Team/TeamOverlay.h"

void ALobbyHUD::SetTeamOverlay()
{
	PlayerController = PlayerController == nullptr ? GetOwningPlayerController() : PlayerController;
	TeamOverlay = TeamOverlay == nullptr ? CreateWidget<UTeamOverlay>(PlayerController, TeamOverlayClass) : TeamOverlay;
	TeamOverlay->SetTeamPlayerTextArray();
}

void ALobbyHUD::SetPickOverlay()
{
	PickOverlay->SetPlayerPickWidgetArray();
}

void ALobbyHUD::CreatePickOverlay()
{
	TeamOverlay->RemoveFromParent();

	PickOverlay = CreateWidget<UPickOverlay>(PlayerController, PickOverlayClass);
	PickOverlay->AddToViewport();

	TArray<UPlayerPickWidget*> PlayerPickWidgetArray = PickOverlay->GetPlayerPickWidgetArray();
	ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(this));
	TArray<ALobbyPlayerState*> PlayerStateArray = LobbyGameState->GetPlayerStateArray();

	int i = 0;
	int j = 1;
	for (auto& iter : PlayerStateArray)
	{
		switch (iter->GetTeam())
		{
		case ETeam::ET_RedTeam:
			PlayerPickWidgetArray[i]->SetPlayerState(iter);
			i += 2;
			break;
		case ETeam::ET_BlueTeam:
			PlayerPickWidgetArray[j]->SetPlayerState(iter);
			j += 2;
			break;
		default:
			break;
		}
	}
}

void ALobbyHUD::SetPickOverlayCountDown(int _CountDown)
{
	FString CountdownText = FString::Printf(TEXT("%d"), _CountDown);
	PickOverlay->SetCountDownText(CountdownText);
}

void ALobbyHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = PlayerController == nullptr ? GetOwningPlayerController() : PlayerController;
	TeamOverlay = TeamOverlay == nullptr ? CreateWidget<UTeamOverlay>(PlayerController, TeamOverlayClass) : TeamOverlay;
	TeamOverlay->AddToViewport();
}