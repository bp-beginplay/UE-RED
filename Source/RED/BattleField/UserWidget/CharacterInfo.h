// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CharacterInfo.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UCharacterInfo : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UProgressBar* HealthBar;

	UPROPERTY(meta = (BindWidget))
	class UProgressBar* ShieldBar;

	//UPROPERTY(meta = (BindWidget))
	//UProgressBar* ManaBar;

	UPROPERTY(meta = (BindWidget))
	class UImage* CharacterImage;

	UPROPERTY(meta = (BindWidget))
	class USkillIcon* PassiveIcon;

	UPROPERTY(meta = (BindWidget))
	USkillIcon* QSkillIcon;

	UPROPERTY(meta = (BindWidget))
	USkillIcon* WSkillIcon;

	UPROPERTY(meta = (BindWidget))
	USkillIcon* ESkillIcon;

	UPROPERTY(meta = (BindWidget))
	USkillIcon* RSkillIcon;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* HealthText;

	//UPROPERTY(meta = (BindWidget))
	//UTextBlock* ManaText;

	// 마나 주석 풀면 같이 해야할것
	// void ABattleFieldHUD::SetManaProgressBar(float MaxMana, float Mana)
};
