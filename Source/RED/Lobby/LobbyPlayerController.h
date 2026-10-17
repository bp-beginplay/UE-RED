// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "LobbyPlayerController.generated.h"

UCLASS()
class RED_API ALobbyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable)
	void Server_AddReadyPlayerCount();

	void SetHUDTeamOverlay();
	void SetHUDPickOverlay();
	void SetHUDPickOverlayCountDown();
	UFUNCTION(Client, Reliable)
	void Client_SetCanPickCharacter(bool _bool);



protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void ReceivedPlayer() override;

private:
	UFUNCTION(Client, Reliable)
	void Client_CreatePickOverlay();


	void CheckTimeSync(float DeltaTime);
	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);
	virtual float GetServerTime();

private:
	UPROPERTY()
	class ALobbyHUD* LobbyHUD;

	int PickCountDown = 3;

	bool IsActivePickOverlay = false;

	float ClientServerDelta = 0.f;
	float SingleTripTime = 0.f;
	float TimeSyncFrequency = 5.f;
	float TimeSyncRunningTime = 0.f;

	float PickStartTime = 0;

public:
	UFUNCTION(Client, Reliable)
	void SetIsActivePickOverlay(bool _Active);
};
