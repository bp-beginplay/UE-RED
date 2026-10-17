// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RED/Info/CharacterListRow.h"
#include "RED/Info/EnumInfo.h"
#include "BattleFieldHUD.generated.h"

UCLASS()
class RED_API ABattleFieldHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void SetHealthProgressBar(float MaxHealth, float Health, float ShieldAmount);
	void SetManaProgressBar(float MaxMana, float Mana);
	void SetCharacterImage(FCharacterListRow* CharacterListRow);
	void SetTeamScore(int RedTeamScore, int BlueTeamScore);
	void AddWinnerAnnoucement(ETeam _Team);
	void FadeOut(bool _IsDied);
	void CreateSkillDescription(FText _Description);
	void DestrorySkillDescription();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY()
	class UPlayerOverlay* PlayerOverlay;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UAnnounceWinner> AnnounceWinnerClass;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class USkillDescriptionUserWidget> SkillDescriptionUserWidgetClass;

	UPROPERTY()
	USkillDescriptionUserWidget* SkillDescriptionUserWidget;

private:
	UPROPERTY()
	class APlayerController* PlayerController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UPlayerOverlay> PlayerOverlayClass;
};
