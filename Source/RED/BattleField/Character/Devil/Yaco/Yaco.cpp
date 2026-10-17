// Fill out your copyright notice in the Description page of Project Settings.


#include "Yaco.h"

#include "YacoSkill_Q.h"
#include "YacoSkill_W.h"
#include "YacoSkill_E.h"
#include "YacoSkill_R.h"
#include "RED/BattleField/BattleFieldGameState.h"
#include "RED/BattleField/BattleFieldHUD.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/UserWidget/CharacterInfo.h"
#include "RED/BattleField/UserWidget/PlayerOverlay.h"
#include "RED/BattleField/UserWidget/SkillIcon.h"

AYaco::AYaco()
{
	Q_SkillComponent = CreateDefaultSubobject<UYacoSkill_Q>(TEXT("Q_SkillComponent"));
	Q_SkillComponent->SetIsReplicated(true);

	W_SkillComponent = CreateDefaultSubobject<UYacoSkill_W>(TEXT("W_SkillComponent"));
	W_SkillComponent->SetIsReplicated(true);

	E_SkillComponent = CreateDefaultSubobject<UYacoSkill_E>(TEXT("E_SkillComponent"));
	E_SkillComponent->SetIsReplicated(true);

	R_SkillComponent = CreateDefaultSubobject<UYacoSkill_R>(TEXT("R_SkillComponent"));
	R_SkillComponent->SetIsReplicated(true);
}

void AYaco::BeginPlay()
{
	Super::BeginPlay();

	CachedGameState = GetWorld()->GetGameState<ABattleFieldGameState>();
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

void AYaco::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

void AYaco::PassiveSkill(float OverFlowHP)
{
	//본인의 체력이 최대체력을 넘어가면 그 남은 체력을 주변 아군중 가장 피가 적은 대상에게 줌

	TArray<ABattleFieldPlayerState*> CloseAllyPlayerList;

	for (APlayerState* PS : CachedGameState->PlayerArray)
	{
		if (!IsValid(PS))
		{
			continue;
		}

		if (PS == GetPlayerState())
		{
			continue;
		}

		if (IsEnemy(PS))
		{
			continue;
		}

		APlayerCharacter* AllyPlayer = Cast<APlayerCharacter>(PS->GetPawn());

		if (!AllyPlayer)
		{
			continue;
		}

		float DistSq = FVector::DistSquared(GetActorLocation(), AllyPlayer->GetActorLocation());

		if (DistSq <= PassiveRange * PassiveRange)
		{
			ABattleFieldPlayerState* BPS = Cast<ABattleFieldPlayerState>(PS);
			CloseAllyPlayerList.Add(BPS);
		}
	}

	ABattleFieldPlayerState* tempAlly = nullptr;
	float HPPercent = 100.f;
	for (auto AllyPlayer : CloseAllyPlayerList)
	{
		float tempHPPercent = AllyPlayer->Health / AllyPlayer->MaxHealth;
		if (tempHPPercent <= HPPercent)
		{
			HPPercent = tempHPPercent;
			tempAlly = AllyPlayer;
		}
	}

	if (tempAlly)
	{
		tempAlly->HealHealth(OverFlowHP);
	}
}

void AYaco::Q_Skill()
{
	Super::Q_Skill();

	if (W_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}
	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (Q_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		float Scale = Q_SkillComponent->ArrangeRadius / 50.f;
		SkillRange1->SetHiddenInGame(false);
		SkillRange2->SetHiddenInGame(false);
		SkillRange1->SetRelativeScale3D(FVector{ Scale ,Scale ,Scale });
		Q_SkillComponent->SkillSetTarget();
	}
}

void AYaco::W_Skill()
{
	Super::W_Skill();

	if (Q_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		Q_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}
	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (W_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		float Scale = W_SkillComponent->ArrangeRadius / 50.f;
		SkillRange1->SetHiddenInGame(false);
		SkillRange2->SetHiddenInGame(false);
		SkillRange1->SetRelativeScale3D(FVector{ Scale ,Scale ,Scale });
		W_SkillComponent->SkillSetTarget();
	}
}

void AYaco::E_Skill()
{
	Super::E_Skill();

	if (Q_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		Q_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}
	if (W_SkillComponent->GetSkillState() != ESkillState::Cooldown)
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

void AYaco::R_Skill()
{
	Super::R_Skill();

	if (Q_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		Q_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (W_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	if (R_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		R_SkillComponent->SkillActive();
	}
}

void AYaco::LMB_Pressed()
{
	Super::LMB_Pressed();
}

void AYaco::RMB_Pressed()
{
	Super::RMB_Pressed();

	if (Q_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		Q_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}
	if (W_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		W_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}
	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->Server_ChangeSkillState(ESkillState::CanActive);
	}

	SkillRange1->SetHiddenInGame(true);
	SkillRange2->SetHiddenInGame(true);
}

void AYaco::LMB_Released(FVector _TargetPosition, AActor* _Actor)
{
	Super::LMB_Released(_TargetPosition, _Actor);

	if (Q_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		Q_SkillComponent->SkillStart(_Actor);
	}

	if (W_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		W_SkillComponent->SkillStart(_TargetPosition);
	}

	if (E_SkillComponent->GetSkillState() != ESkillState::Cooldown)
	{
		E_SkillComponent->SkillStart(_Actor);
	}

	SkillRange1->SetHiddenInGame(true);
	SkillRange2->SetHiddenInGame(true);
}

void AYaco::RMB_Released()
{
	Super::RMB_Released();
}

void AYaco::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	ABattleFieldPlayerController* BPC = Cast<ABattleFieldPlayerController>(NewController);
	Q_SkillComponent->SetPC(BPC);
	E_SkillComponent->SetPC(BPC);
	R_SkillComponent->SetPC(BPC);

	if (ABattleFieldPlayerState* BPS = GetPlayerState<ABattleFieldPlayerState>())
	{
		BPS->OnHpChanged.AddUObject(this, &AYaco::PassiveSkill);
	}
}
