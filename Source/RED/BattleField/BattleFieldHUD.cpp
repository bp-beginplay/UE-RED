// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleFieldHUD.h"

#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Styling/SlateBrush.h"

#include "UserWidget/CharacterInfo.h"
#include "UserWidget/PlayerOverlay.h"
#include "UserWidget/SkillIcon.h"
#include "UserWidget/TeamScore.h"
#include "UserWidget/AnnounceWinner.h"
#include "UserWidget/SkillDescriptionUserWidget.h"
#include "RED/BattleField/BattleFieldPlayerState.h"

void ABattleFieldHUD::SetHealthProgressBar(float MaxHealth, float Health, float ShieldAmount)
{
	PlayerController = PlayerController == nullptr ? GetOwningPlayerController() : PlayerController;

	PlayerOverlay = PlayerOverlay == nullptr ? CreateWidget<UPlayerOverlay>(PlayerController, PlayerOverlayClass) : PlayerOverlay;

	PlayerOverlay->CharacterInfo->HealthBar->SetPercent(Health / MaxHealth);
	PlayerOverlay->CharacterInfo->ShieldBar->SetPercent(ShieldAmount / MaxHealth);

	FString HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));

	PlayerOverlay->CharacterInfo->HealthText->SetText(FText::FromString(HealthText));
}

void ABattleFieldHUD::SetManaProgressBar(float MaxMana, float Mana)
{
	//PlayerOverlay->CharacterInfo->ManaBar->SetPercent(Mana / MaxMana);
	FString ManaText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Mana), FMath::CeilToInt(MaxMana));
	//PlayerOverlay->CharacterInfo->ManaText->SetText(FText::FromString(ManaText));
}

void ABattleFieldHUD::SetCharacterImage(FCharacterListRow* CharacterListRow)
{
	FSlateBrush CharacterBrush;
	CharacterBrush.SetResourceObject(CharacterListRow->CharacterImage);
	FSlateBrush PassiveBrush;
	PassiveBrush.SetResourceObject(CharacterListRow->PassiveImage);
	FSlateBrush QSkillBrush;
	QSkillBrush.SetResourceObject(CharacterListRow->QSkillImage);
	FSlateBrush WSkillBrush;
	WSkillBrush.SetResourceObject(CharacterListRow->WSkillImage);
	FSlateBrush ESkillBrush;
	ESkillBrush.SetResourceObject(CharacterListRow->ESkillImage);
	FSlateBrush RSkillBrush;
	RSkillBrush.SetResourceObject(CharacterListRow->RSkillImage);

	PlayerController = GetOwningPlayerController();
	PlayerOverlay = PlayerOverlay == nullptr ? CreateWidget<UPlayerOverlay>(PlayerController, PlayerOverlayClass) : PlayerOverlay;
	PlayerOverlay->CharacterInfo->CharacterImage->SetBrush(CharacterBrush);
	PlayerOverlay->CharacterInfo->PassiveIcon->SkillImage->SetBrush(PassiveBrush);
	PlayerOverlay->CharacterInfo->QSkillIcon->SkillImage->SetBrush(QSkillBrush);
	PlayerOverlay->CharacterInfo->WSkillIcon->SkillImage->SetBrush(WSkillBrush);
	PlayerOverlay->CharacterInfo->ESkillIcon->SkillImage->SetBrush(ESkillBrush);
	PlayerOverlay->CharacterInfo->RSkillIcon->SkillImage->SetBrush(RSkillBrush);

	PlayerOverlay->CharacterInfo->PassiveIcon->SkillDescription = CharacterListRow->PassiveDescription;
	PlayerOverlay->CharacterInfo->QSkillIcon->SkillDescription = CharacterListRow->QSkillDescription;
	PlayerOverlay->CharacterInfo->WSkillIcon->SkillDescription = CharacterListRow->WSkillDescription;
	PlayerOverlay->CharacterInfo->ESkillIcon->SkillDescription = CharacterListRow->ESkillDescription;
	PlayerOverlay->CharacterInfo->RSkillIcon->SkillDescription = CharacterListRow->RSkillDescription;
}

void ABattleFieldHUD::SetTeamScore(int RedTeamScore, int BlueTeamScore)
{
	FString TeamScoreText = FString::Printf(TEXT("%d : %d"), RedTeamScore, BlueTeamScore);
	PlayerOverlay->TeamScore->TeamScoreTextBlock->SetText(FText::FromString(TeamScoreText));
}

void ABattleFieldHUD::AddWinnerAnnoucement(ETeam _Team)
{
	UAnnounceWinner* ElimAnnouncementWidget = CreateWidget<UAnnounceWinner>(PlayerController, AnnounceWinnerClass);
	FString ElimAnnouncementText;
	switch (_Team)
	{
	case ETeam::ET_RedTeam:
		 ElimAnnouncementText = FString::Printf(TEXT("Winner : RedTeam"));
		break;
	case ETeam::ET_BlueTeam:
		ElimAnnouncementText = FString::Printf(TEXT("Winner : BlueTeam"));
		break;
	case ETeam::ET_NoTeam:
		ElimAnnouncementText = FString::Printf(TEXT("None"));
		break;
	default:
		break;
	}
	ElimAnnouncementWidget->WinnerTextBlock->SetText(FText::FromString(ElimAnnouncementText));
	ElimAnnouncementWidget->AddToViewport();
}

void ABattleFieldHUD::FadeOut(bool _IsDied)
{
	if (_IsDied)
	{
		PlayerOverlay->FadeOutImage->SetColorAndOpacity(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f));
	}
	else
	{
		PlayerOverlay->FadeOutImage->SetColorAndOpacity(FLinearColor(0.0f, 0.0f, 0.0f, 0.0f));
	}
}

void ABattleFieldHUD::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = PlayerController == nullptr ? GetOwningPlayerController() : PlayerController;
	PlayerOverlay = PlayerOverlay == nullptr ? CreateWidget<UPlayerOverlay>(PlayerController, PlayerOverlayClass) : PlayerOverlay;
	PlayerOverlay->AddToViewport();

	SetTeamScore(1, 1);
}

void ABattleFieldHUD::CreateSkillDescription(FText _Description)
{
	SkillDescriptionUserWidget = CreateWidget<USkillDescriptionUserWidget>(PlayerController, SkillDescriptionUserWidgetClass);
	SkillDescriptionUserWidget->AddToViewport();
	SkillDescriptionUserWidget->SkillDescriptionTextBlock->SetText(_Description);
}

void ABattleFieldHUD::DestrorySkillDescription()
{
	SkillDescriptionUserWidget->RemoveFromParent();
}
