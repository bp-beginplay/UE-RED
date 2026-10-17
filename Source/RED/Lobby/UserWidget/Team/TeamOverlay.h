// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TeamOverlay.generated.h"

UCLASS()
class RED_API UTeamOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetTeamPlayerTextArray();

protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
	void ReadyButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton* ReadyButton;

	UPROPERTY(meta = (BindWidget))
	class UTeamButton* RedTeamButton;

	UPROPERTY(meta = (BindWidget))
	UTeamButton* BlueTeamButton;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* NoTeamPlayer1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoTeamPlayer2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoTeamPlayer3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoTeamPlayer4;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoTeamPlayer5;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* NoTeamPlayer6;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamPlayer1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamPlayer2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* RedTeamPlayer3;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamPlayer1;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamPlayer2;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BlueTeamPlayer3;

	UPROPERTY()
	TArray<UTextBlock*> TeamPlayerTextArray;
};
