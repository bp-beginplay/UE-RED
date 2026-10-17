// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerPickWidget.generated.h"

UCLASS()
class RED_API UPlayerPickWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPlayerState(class ALobbyPlayerState* _PlayerState);
	void SetCharacterNameText();

private:
	void SetPlayerNameText();

private:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* PlayerName;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharacterName;

	UPROPERTY(meta = (BindWidget))
	class UImage* CharacterImage;

	UPROPERTY()
	ALobbyPlayerState* LobbyPlayerState;

	UPROPERTY(EditAnywhere)
	int PickSequence;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> CharacterDataTable;

public:
	ALobbyPlayerState* GetLobbyPlayerState() const { return LobbyPlayerState; }
};
