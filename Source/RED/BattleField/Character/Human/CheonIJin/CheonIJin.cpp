// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJin.h"

#include "Components/TextBlock.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Net/UnrealNetwork.h"

#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/BattleFieldHUD.h"
#include "RED/BattleField/UserWidget/PlayerOverlay.h"
#include "RED/BattleField/UserWidget/CharacterInfo.h"
#include "RED/BattleField/UserWidget/SkillIcon.h"

#include "CheonIJinSkill_Q.h"
#include "CheonIJinSkill_W.h"
#include "CheonIJinSkill_E.h"
#include "CheonIJinSkill_R.h"

ACheonIJin::ACheonIJin()
{
	Q_SkillComponent = CreateDefaultSubobject<UCheonIJinSkill_Q>(TEXT("Q_SkillComponent"));
	Q_SkillComponent->SetIsReplicated(true);
	W_SkillComponent = CreateDefaultSubobject<UCheonIJinSkill_W>(TEXT("W_SkillComponent"));
	W_SkillComponent->SetIsReplicated(true);
	E_SkillComponent = CreateDefaultSubobject<UCheonIJinSkill_E>(TEXT("E_SkillComponent"));
	E_SkillComponent->SetIsReplicated(true);
	R_SkillComponent = CreateDefaultSubobject<UCheonIJinSkill_R>(TEXT("R_SkillComponent"));
	R_SkillComponent->SetIsReplicated(true);

	GetCharacterMovement()->BrakingDecelerationFlying = 5000.f;
	GetCharacterMovement()->MaxAcceleration = 8000.f;
	GetCharacterMovement()->BrakingFrictionFactor = 2.0f;

}

void ACheonIJin::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PC = Cast<APlayerController>(GetController());
	ABattleFieldHUD* HUD = nullptr;
	if (PC)
	{
		HUD = Cast<ABattleFieldHUD>(PC->GetHUD());
	}
	if (Q_SkillComponent)
	{
		Q_SkillComponent->PlayerCharacter = this;
		if (HUD)
		{
			Q_SkillComponent->SkillIcon = HUD->PlayerOverlay->CharacterInfo->QSkillIcon;
		}
	}
	if (W_SkillComponent)
	{
		W_SkillComponent->PlayerCharacter = this;
		if (HUD)
		{
			W_SkillComponent->SkillIcon = HUD->PlayerOverlay->CharacterInfo->WSkillIcon;
			W_SkillComponent->SkillIcon->SkillCountTextBlock->SetVisibility(ESlateVisibility::Visible);
		}
	}
	if (E_SkillComponent)
	{
		E_SkillComponent->PlayerCharacter = this;
		if (HUD)
		{
			E_SkillComponent->SkillIcon = HUD->PlayerOverlay->CharacterInfo->ESkillIcon;
		}
	}
	if (R_SkillComponent)
	{
		R_SkillComponent->PlayerCharacter = this;
		if (HUD)
		{
			R_SkillComponent->SkillIcon = HUD->PlayerOverlay->CharacterInfo->RSkillIcon;
		}
	}
}

void ACheonIJin::Q_Skill()
{
	Super::Q_Skill();

	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	//if (W_SkillComponent->GetSkillState() != ESkillState::CanActive)
	//{
	//	W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	//}

	if (R_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		R_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (Q_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		Q_SkillComponent->SkillStart();
	}

	if (W_SkillComponent->GetSkillState() != ESkillState::CanActive)
	{
		W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	SkillRange1->SetHiddenInGame(true);
	SkillRange2->SetHiddenInGame(true);
}

void ACheonIJin::W_Skill()
{
	Super::W_Skill();

	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (R_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		R_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (W_SkillComponent->GetSkillCount() > 0)
	{
		float Scale = W_SkillComponent->ArrangeRadius / 50.f;
		SkillRange1->SetHiddenInGame(false);
		SkillRange2->SetHiddenInGame(false);
		SkillRange1->SetRelativeScale3D(FVector{ Scale ,Scale ,Scale });
		W_SkillComponent->SkillSetTarget();
	}
	
}

void ACheonIJin::E_Skill()
{
	Super::E_Skill();

	if (R_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		R_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (W_SkillComponent->GetSkillState() != ESkillState::CanActive)
	{
		W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		float Scale = E_SkillComponent->ArrangeRadius / 50.f;
		SkillRange1->SetHiddenInGame(false);
		SkillRange2->SetHiddenInGame(false);
		SkillRange1->SetRelativeScale3D(FVector{ Scale ,Scale ,Scale });
		E_SkillComponent->SkillSetTarget();
	}
}

void ACheonIJin::R_Skill()
{
	Super::R_Skill();

	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (W_SkillComponent->GetSkillState() != ESkillState::CanActive)
	{
		W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (R_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		float Scale = R_SkillComponent->ArrangeRadius / 50.f;
		SkillRange1->SetHiddenInGame(false);
		SkillRange2->SetHiddenInGame(false);
		SkillRange1->SetRelativeScale3D(FVector{ Scale ,Scale ,Scale });
		R_SkillComponent->SkillSetTarget();
	}

	//if (W_SkillComponent->GetSkillState() != ESkillState::CanActive)
	//{
	//	W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	//}
}

void ACheonIJin::LMB_Released(FVector _TargetPosition, AActor* _Actor)
{
	Super::LMB_Released(_TargetPosition, _Actor);
	W_SkillComponent->SkillStart(_TargetPosition);
	E_SkillComponent->SkillStart(_TargetPosition);
	R_SkillComponent->SkillStart(_TargetPosition);

	SkillRange1->SetHiddenInGame(true);
	SkillRange2->SetHiddenInGame(true);
}

void ACheonIJin::RMB_Pressed()
{
	Super::RMB_Pressed();

	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (R_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		R_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	SkillRange1->SetHiddenInGame(true);
	SkillRange2->SetHiddenInGame(true);
}
