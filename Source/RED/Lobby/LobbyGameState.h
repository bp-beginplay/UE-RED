// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "LobbyGameState.generated.h"

UCLASS()
class RED_API ALobbyGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	void AddPlayerStateArray(class ALobbyPlayerState* _NewPlayerName);

	bool AddReadyPlayer();

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_PlayerStateArray();

private:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_PlayerStateArray, meta = (AllowPrivateAccess = "true"))
	TArray<ALobbyPlayerState*>PlayerStateArray;

	int ReadyPlayer = 0;

public:
	TArray<ALobbyPlayerState*> GetPlayerStateArray() { return PlayerStateArray; }
};
