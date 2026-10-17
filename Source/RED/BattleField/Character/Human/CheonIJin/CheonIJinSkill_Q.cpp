// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJinSkill_Q.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/UserWidget/SkillIcon.h"

void UCheonIJinSkill_Q::SkillCanActive()
{
	Client_SetSkillIconOpacity(true);
}

void UCheonIJinSkill_Q::SkillStart()
{
	Super::SkillStart();
}

void UCheonIJinSkill_Q::SkillActive()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->GetCharacterMovement()->GravityScale = 0.f;
		PlayerCharacter->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Flying;
		PlayerCharacter->GetCharacterMovement()->SetPlaneConstraintEnabled(true);
		PlayerCharacter->GetCharacterMovement()->SetPlaneConstraintNormal(FVector::UpVector);
		PlayerCharacter->Ground = EGround::OnAir;
	}

	PlayAnimMontage(SkillActiveMontage);

	GetWorld()->GetTimerManager().SetTimer(
		FlyJumpTHandle,
		this,
		&UCheonIJinSkill_Q::FlyJump,
		0.01f,
		true
	);

	Client_UpdateCoolTimeTextBlock(ActiveTime);
	Client_ChangeSkillState(ESkillState::Cooldown, ActiveTime);
}

void UCheonIJinSkill_Q::SkillCooldown()
{
	if (PlayerCharacter && !PlayerCharacter->IsDied)
	{
		PlayerCharacter->GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Falling;
		PlayerCharacter->GetCharacterMovement()->GravityScale = 1.f;
		PlayerCharacter->GetCharacterMovement()->SetPlaneConstraintEnabled(false);
		PlayerCharacter->Ground = EGround::Ground;
	}

	Client_SetSkillIconOpacity(false);
	Client_UpdateCoolTimeTextBlock(CoolTime);
	Client_ChangeSkillState(ESkillState::CanActive, CoolTime);
}

void UCheonIJinSkill_Q::FlyJump()
{
	FVector FlyLocation = PlayerCharacter->GetActorLocation();
	FlyLocation.Z += 4.f;
	PlayerCharacter->SetActorLocation(FlyLocation);

	if (FlyLocation.Z >= 400.f)
	{
		GetWorld()->GetTimerManager().ClearTimer(FlyJumpTHandle);
	}
}