// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillIcon.h"

#include "Components/TextBlock.h"
#include "Components/Button.h"

#include "RED/BattleField/BattleFieldHUD.h"

bool USkillIcon::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	CoolTimeTextBlock->SetVisibility(ESlateVisibility::Hidden);
	SkillCountTextBlock->SetVisibility(ESlateVisibility::Hidden);

	if (SkillDescriptionButton)
	{
		SkillDescriptionButton->OnHovered.AddDynamic(this, &USkillIcon::ShowSkillDescription);
		SkillDescriptionButton->OnUnhovered.AddDynamic(this, &USkillIcon::HideSkillDescription);
	}


	return true;
}

void USkillIcon::ShowSkillDescription()
{
	ABattleFieldHUD* BattleFieldHUD = Cast<ABattleFieldHUD>(GetOwningPlayer()->GetHUD());
	BattleFieldHUD->CreateSkillDescription(SkillDescription);
}

void USkillIcon::HideSkillDescription()
{
	ABattleFieldHUD* BattleFieldHUD = Cast<ABattleFieldHUD>(GetOwningPlayer()->GetHUD());
	BattleFieldHUD->DestrorySkillDescription();
}
