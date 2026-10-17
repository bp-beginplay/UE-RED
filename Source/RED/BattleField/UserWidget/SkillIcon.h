// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SkillIcon.generated.h"

UCLASS()
class RED_API USkillIcon : public UUserWidget
{
	GENERATED_BODY()

	virtual bool Initialize() override;

	UFUNCTION()
	void ShowSkillDescription();
	UFUNCTION()
	void HideSkillDescription();

public:
	UPROPERTY(meta = (BindWidget))
	class UImage* SkillImage;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CoolTimeTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SkillCountTextBlock;

	UPROPERTY(meta = (BindWidget))
	class UButton* SkillDescriptionButton;

	FText SkillDescription = FText();
};
