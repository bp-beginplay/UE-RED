// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "RED/Info/EnumInfo.h"
#include "BattleFieldGameMode.generated.h"

UCLASS()
class RED_API ABattleFieldGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ABattleFieldGameMode();
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController, int32 RespawnTargetIdx);

protected:
	UFUNCTION(BlueprintCallable)
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	virtual void BeginPlay() override;

public:
	virtual void UpdateTeamScore(ETeam _Team, int _Score);
	void EndGame();

	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> CharacterDataTable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<class AControlPointBaseActor*> ControlPointList;

public:
	UFUNCTION()
	const TArray<class AControlPointBaseActor*>& GetControlPointList() const
	{
		return ControlPointList;
	}

	FTimerHandle FirstTimerHandle;

	UPROPERTY()
	TArray<AController*> LoginPlayerList;

	virtual void HandleSeamlessTravelPlayer(AController*& C) override;

	int32 ReadyPlayerCount = 0;
	int32 ExpectedPlayerCount = 0;
	void NotifyPlayerReady();
	void AllPlayersReady();

	int RedTeamScore = 1.f;
	int BlueTeamScore = 1.f;

	bool GameStart = false;

	float GameTime = 0.f;

	UFUNCTION()
	void IncreaseGameTime();
};