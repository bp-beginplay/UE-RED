// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RED/Info/EnumInfo.h"
#include "TeamButton.generated.h"

UCLASS()
class RED_API UTeamButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual bool Initialize() override;

private:
	UFUNCTION()
	void TeamButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UButton * TeamButton;

	UPROPERTY(EditAnywhere)
	ETeam Team = ETeam::ET_NoTeam;
};
