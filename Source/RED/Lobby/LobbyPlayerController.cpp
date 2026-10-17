// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyPlayerController.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "LobbyGameState.h"
#include "LobbyHUD.h"
#include "LobbyGameMode.h"
#include "LobbyPlayerState.h"
#include "UserWidget/Pick/PickOverlay.h"

void ALobbyPlayerController::SetHUDTeamOverlay()
{
	LobbyHUD = LobbyHUD == nullptr ? Cast<ALobbyHUD>(GetHUD()) : LobbyHUD;
	LobbyHUD->SetTeamOverlay();
}

void ALobbyPlayerController::SetHUDPickOverlay()
{
	LobbyHUD->SetPickOverlay();
}

void ALobbyPlayerController::SetHUDPickOverlayCountDown()
{
	LobbyHUD->SetPickOverlayCountDown(PickCountDown - (int(GetServerTime() - PickStartTime) % PickCountDown));
}

void ALobbyPlayerController::Client_SetCanPickCharacter_Implementation(bool _bool)
{
	LobbyHUD->GetPickOverlay()->SetCharacterButtonEnable(_bool);
}

void ALobbyPlayerController::Server_AddReadyPlayerCount_Implementation()
{
	ALobbyGameState* LobbyGameState = Cast<ALobbyGameState>(UGameplayStatics::GetGameState(this));
	if (LobbyGameState->AddReadyPlayer())
	{
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ALobbyPlayerController* LobbyPlayerController = Cast<ALobbyPlayerController>(*It);
			LobbyPlayerController->Client_CreatePickOverlay();
		}
		ALobbyGameMode* LobbyGameMode = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
		LobbyGameMode->SetActivePickOverlay(true);
	}
}

void ALobbyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);

	LobbyHUD = Cast<ALobbyHUD>(GetHUD());
}

void ALobbyPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckTimeSync(DeltaTime);

	if(IsActivePickOverlay)
	{
		SetHUDPickOverlayCountDown();
	}
}

void ALobbyPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ALobbyPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ALobbyPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ALobbyPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = 0.5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ALobbyPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	else return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ALobbyPlayerController::Client_CreatePickOverlay_Implementation()
{
	PickStartTime = GetServerTime();
	LobbyHUD->CreatePickOverlay();
	IsActivePickOverlay = true;
}

void ALobbyPlayerController::SetIsActivePickOverlay_Implementation(bool _Active)
{
	IsActivePickOverlay = _Active;
}