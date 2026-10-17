// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"

UCLASS()
class RED_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
	ALobbyGameMode();
	void ServerTravel();

protected:
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

private:
	float PickStartTime = 0;
	bool IsActivePickOverlay = false;
	int PickSequence = 0;
	bool ActiveOnce = true;
	int PickCountDown = 3;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> CharacterDataTable;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int PlayerCount = 0;

public:
	void SetActivePickOverlay(bool _IsActive);
};
