// Fill out your copyright notice in the Description page of Project Settings.

#include "SelectControlZoneUI.h"
#include "Components/Button.h"

#include "RED/BattleField/BattleFieldPlayerState.h"

void USelectControlZoneUI::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FButtonStyle Style = Zone1->GetStyle();
	Style.Normal.TintColor = FSlateColor(FLinearColor(0.6f, 0.6f, 0.6f, 1.0f));
	Zone1->SetStyle(Style);
	Zone2->SetStyle(Style);
	Zone3->SetStyle(Style);
	Zone4->SetStyle(Style);
	Zone5->SetStyle(Style);
	Zone6->SetStyle(Style);
	Zone7->SetStyle(Style);
}

void USelectControlZoneUI::SetButtonColor(UButton* Button, ETeam Team)
{
	switch (Team)
	{
	case ETeam::ET_BlueTeam:
		Button->SetBackgroundColor(FLinearColor{ 0, 0, 1, 1 });
		break;
	case ETeam::ET_RedTeam:
		Button->SetBackgroundColor(FLinearColor{ 1, 0, 0, 1 });
		break;
	case ETeam::ET_NoTeam:
		Button->SetBackgroundColor(FLinearColor{ 1, 1, 1, 1 });
		break;
	default:
		break;
	}
}

void USelectControlZoneUI::ChangeButtonColor(int32 ButtonIdx, ETeam Team)
{
	// 이것도 사실상 임시방편
	if (!this)
	{
		return;
	}

	switch (ButtonIdx)
	{
	case 1:
		SetButtonColor(Zone1, Team);
		break;
	case 2:
		SetButtonColor(Zone2, Team);
		break;
	case 3:
		SetButtonColor(Zone3, Team);
		break;
	case 4:
		SetButtonColor(Zone4, Team);
		break;
	case 5:
		SetButtonColor(Zone5, Team);
		break;
	case 6:
		SetButtonColor(Zone6, Team);
		break;
	case 7:
		SetButtonColor(Zone7, Team);
		break;
	default:
		break;
	}
}

bool USelectControlZoneUI::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HealthUpgradeButton)
		HealthUpgradeButton->OnClicked.AddDynamic(this, &ThisClass::UpgradeHealth);
	if (AttackUpgradeButton)
		AttackUpgradeButton->OnClicked.AddDynamic(this, &ThisClass::UpgradeAttack);
	return true;
}

void USelectControlZoneUI::UpgradeHealth()
{
	ABattleFieldPlayerState* PS = GetWorld()->GetFirstPlayerController()->GetPlayerState<ABattleFieldPlayerState>();
	PS->Server_UpgradeMaxHealth(10.f);
	PS->OnRep_Health();
	PS->OnRep_Point();
}

void USelectControlZoneUI::UpgradeAttack()
{
	ABattleFieldPlayerState* PS = GetWorld()->GetFirstPlayerController()->GetPlayerState<ABattleFieldPlayerState>();
	PS->Server_AddBaseState(5.f);
	PS->OnRep_Point();
}
