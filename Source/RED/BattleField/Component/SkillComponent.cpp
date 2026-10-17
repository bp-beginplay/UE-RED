// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillComponent.h"

#include "Net/UnrealNetwork.h"
#include "Components/TextBlock.h"

#include "Components/Image.h"

#include "RED/BattleField/UserWidget/SkillIcon.h"
#include "RED/BattleField/Character/PlayerCharacter.h"

void USkillComponent::AutoRun()
{

}

void USkillComponent::Client_SetSkillIconOpacity_Implementation(bool _CanActive)
{
	if (!SkillIcon)
		return;

	if (_CanActive)
	{
		SkillIcon->SkillImage->SetColorAndOpacity(FLinearColor(1.f, 1.f, 1.f, 1.f));
	}
	else
	{
		SkillIcon->SkillImage->SetColorAndOpacity(FLinearColor(0.2f, 0.2f, 0.2f, 1.f));
	}
}

void USkillComponent::SkillSetTarget()
{
	if (SkillState != ESkillState::CanActive)
		return;
	Server_ChangeSkillState(ESkillState::SetTarget);
}

void USkillComponent::SkillStart()
{
	if (SkillState != ESkillState::CanActive)
		return;
	Server_ChangeSkillState(ESkillState::Active);
}

void USkillComponent::SkillStart(AActor* _Actor)
{
	if (SkillState != ESkillState::SetTarget)
		return;
	Server_SetTargetActor(_Actor);
	Server_ChangeSkillState(ESkillState::Active);
}

void USkillComponent::SkillStart(FVector _TargetLocation)
{
	Server_SetTargetLocation(_TargetLocation);
	Server_ChangeSkillState(ESkillState::Active);
}

void USkillComponent::Server_SetTargetActor_Implementation(AActor* _TargetActor)
{
	TargetActor = _TargetActor;
}

void USkillComponent::SkillActive()
{

}

void USkillComponent::SkillCooldown()
{

}

void USkillComponent::SkillCanActive()
{

}

void USkillComponent::Server_SetbAutoRun_Implementation(bool _bAutoRun)
{
	bAutoRun = _bAutoRun;
}

void USkillComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(USkillComponent, SkillState);
	DOREPLIFETIME(USkillComponent, SkillCount);
	DOREPLIFETIME(USkillComponent, TargetLocation);
	DOREPLIFETIME(USkillComponent, TargetActor);
	DOREPLIFETIME(USkillComponent, bAutoRun);
}

void USkillComponent::Server_Fire_Implementation()
{
}

void USkillComponent::EndAnimMontage()
{
	bPlayAnimMontage = false;
	Server_ChangeSkillState(ESkillState::CanActive);
}

void USkillComponent::OnRep_SkillState()
{
	switch (SkillState)
	{
	case ESkillState::SetTarget:
		break;
	case ESkillState::CanActive:
		SkillCanActive();
		break;
	case ESkillState::Active:
		SkillActive();
		break;
	case ESkillState::Cooldown:
		SkillCooldown();
		break;
	default:
		break;
	}
}

void USkillComponent::Server_UpdateSkillCountTextBlock_Implementation()
{
	UpdateSkillCountTextBlock();
}

void USkillComponent::PlayAnimMontage(UAnimMontage* _Montage)
{
	if (!PlayerCharacter)
	{
		return;
	}
	bPlayAnimMontage = true;
	UAnimInstance* AnimInstance = PlayerCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance && _Montage)
	{
		AnimInstance->Montage_Play(_Montage);
	}
}


void USkillComponent::Client_ChangeSkillState_Implementation(ESkillState _SkillState, float _Time)
{
	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle,
		FTimerDelegate::CreateLambda([this, _SkillState]()
			{
				Server_ChangeSkillState(_SkillState);
			}),
		_Time,
		false
	);
}

void USkillComponent::Client_UpdateCoolTimeTextBlock_Implementation(float _CoolTime)
{
	CountDown = _CoolTime;
	if (SkillIcon)
		SkillIcon->CoolTimeTextBlock->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(
		CoolTimeTimerHandle,
		this,
		&USkillComponent::UpdateCoolTimeTextBlock,
		0.1f,
		true
	);
}

void USkillComponent::UpdateCoolTimeTextBlock()
{
	if (SkillIcon)
	{
		CountDown -= 0.1;
		SkillIcon->CoolTimeTextBlock->SetText(FText::AsNumber(CountDown));
		if (CountDown <= 0.f)
		{
			GetWorld()->GetTimerManager().ClearTimer(CoolTimeTimerHandle);
			SkillIcon->CoolTimeTextBlock->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void USkillComponent::OnRep_SkillCount()
{
	UpdateSkillCountTextBlock();
}

void USkillComponent::UpdateSkillCountTextBlock()
{
	if (!SkillIcon) return;
	
	SkillIcon->SkillCountTextBlock->SetText(FText::AsNumber(SkillCount));
	Client_SetSkillIconOpacity(SkillCount != 0);
}



void USkillComponent::Server_AddSkillCount_Implementation()
{
	SkillCount += 1;
	Server_UpdateSkillCountTextBlock();
	if (SkillCount != InitSkillCount)
	{
		GetWorld()->GetTimerManager().ClearTimer(SkillCountTimerHandle);
		Client_SkillCountTime();
	}
}

void USkillComponent::Client_SkillCountTime_Implementation()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(SkillCountTimerHandle))
	{
		CountDown = CoolTime;
		GetWorld()->GetTimerManager().SetTimer(
			SkillCountTimerHandle,
			this,
			&USkillComponent::Server_AddSkillCount,
			CoolTime,
			false
		);
	}
	
	if (SkillIcon)
		SkillIcon->CoolTimeTextBlock->SetVisibility(ESlateVisibility::Visible);
	GetWorld()->GetTimerManager().SetTimer(
		CoolTimeTimerHandle,
		this,
		&USkillComponent::UpdateCoolTimeTextBlock,
		0.1f,
		true
	);
}

void USkillComponent::Server_SetTargetLocation_Implementation(FVector _TargetLocation)
{
	TargetLocation = _TargetLocation;
}

void USkillComponent::Server_ChangeSkillState_Implementation(ESkillState _SkillState)
{
	SkillState = _SkillState;
	switch (SkillState)
	{
	case ESkillState::SetTarget:
		break;
	case ESkillState::CanActive:
		SkillCanActive();
		break;
	case ESkillState::Active:
		SkillActive();
		break;
	case ESkillState::Cooldown:
		SkillCooldown();
		break;
	default:
		break;
	}
}