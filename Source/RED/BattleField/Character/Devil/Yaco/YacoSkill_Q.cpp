// Fill out your copyright notice in the Description page of Project Settings.


#include "YacoSkill_Q.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/Info/StructInfo.h"

UYacoSkill_Q::UYacoSkill_Q()
{
	PrimaryComponentTick.bCanEverTick = true;
	CoolTime = 5;
}

void UYacoSkill_Q::SkillStart()
{
	Super::SkillStart();
}

void UYacoSkill_Q::SkillStart(AActor* _Actor)
{
	if (_Actor)
	{
		APlayerCharacter* Player = Cast<APlayerCharacter>(_Actor);
		if (!Player)
		{
			Server_ChangeSkillState(ESkillState::CanActive);
			return;
		}

		if (IActorInterface::Execute_GetETeam(_Actor) != IActorInterface::Execute_GetETeam(GetOwner()))
		{
			Server_ChangeSkillState(ESkillState::CanActive);
			return;
		}

		Super::SkillStart(_Actor);
	}
	else
	{
		Server_ChangeSkillState(ESkillState::CanActive);
	}
}

void UYacoSkill_Q::SkillActive()
{
	Super::SkillActive();

	FVector TargetDistance = TargetActor->GetActorLocation() - PlayerCharacter->GetActorLocation();
	TargetDistance.Z = 0;
	if (ArrangeRadius < TargetDistance.Length())
	{
		bAutoRun = true;
	}
	else
	{
		bAutoRun = false;
		PlayAnimMontage(SkillActiveMontage);
	}
}

void UYacoSkill_Q::SkillCooldown()
{
	Super::SkillCooldown();
}

void UYacoSkill_Q::SkillCanActive()
{
	Super::SkillCanActive();

	Client_SetSkillIconOpacity(true);
}

void UYacoSkill_Q::AutoRun()
{
	Super::AutoRun();

	if (bAutoRun)
	{
		FVector TargetDistance = TargetActor->GetActorLocation() - PlayerCharacter->GetActorLocation();
		TargetDistance.Z = 0;
		const FVector WorldDirection = TargetDistance.GetSafeNormal();
		PlayerCharacter->AddMovementInput(WorldDirection);
		if (ArrangeRadius >= TargetDistance.Length())
		{
			bAutoRun = false;
			PlayAnimMontage(SkillActiveMontage);
		}
	}
}

void UYacoSkill_Q::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AutoRun();
}

void UYacoSkill_Q::SetPC(ABattleFieldPlayerController* PC)
{
	BPC = PC;
}

void UYacoSkill_Q::Server_Fire_Implementation()
{
	Super::Server_Fire_Implementation();

	if (BPC)
	{
		ABattleFieldPlayerState* TargetPS = nullptr;
		ABattleFieldPlayerController* TargetPC = nullptr;
		if (TargetActor)
		{
			TargetPC = Cast<ABattleFieldPlayerController>(TargetActor->GetInstigatorController());

			if (IActorInterface::Execute_GetETeam(TargetActor) != IActorInterface::Execute_GetETeam(GetOwner()))
			{
				return;
			}

			if (TargetPC)
			{
				TargetPS = Cast<ABattleFieldPlayerState>(TargetPC->GetPlayerState<ABattleFieldPlayerState>());
			}

			if (TargetPS)
			{
				TakenHP = TargetPS->Health * 0.2f;
				TargetPS->Health *= 0.8f;

				BPC->GetPlayerState<ABattleFieldPlayerState>()->HealHealth(TakenHP);

				Server_RequestPlayEffect(TargetPS);

				FShieldData NewShield;

				NewShield.ShieldAmount = TakenHP + BasicShieldAmount;
				NewShield.Duration = 3.f;
				NewShield.StartTime = GetWorld()->GetTimeSeconds();
				NewShield.FromSkill = UYacoSkill_Q::StaticClass();
				NewShield.SpawnedEffect = ActiveBuffEffect;

				TargetPS->ActiveShields.Add(NewShield);

				GetWorld()->GetTimerManager().SetTimer(
					NewShield.TimerHandle,
					FTimerDelegate::CreateLambda([this, TargetPS]
						{
							RemoveShield(TargetPS);
						}),
					3.f,
					false
				);

				TargetPS->OnRep_Shield();
				TargetPS->OnRep_Health();
			}

			Server_ChangeSkillState(ESkillState::Cooldown);

			Client_SetSkillIconOpacity(false);
			Client_UpdateCoolTimeTextBlock(CoolTime);
			Client_ChangeSkillState(ESkillState::CanActive, CoolTime);
		}
	}
}

void UYacoSkill_Q::RemoveShield(ABattleFieldPlayerState* TargetPS)
{
	if (!TargetPS) 
	{
		return;
	}

	for (int32 i = 0; i < TargetPS->ActiveShields.Num(); ++i)
	{
		if (TargetPS->ActiveShields[i].FromSkill == UYacoSkill_Q::StaticClass())
		{
			TargetPS->ActiveShields.RemoveAt(i);
			break;
		}
	}

	TargetPS->OnRep_Shield();

	if (ActiveBuffEffect)
	{
		ActiveBuffEffect->Destroy();
		ActiveBuffEffect = nullptr;
	}
}

void UYacoSkill_Q::Server_RequestPlayEffect_Implementation(ABattleFieldPlayerState* targetPS)
{
	Mul_PlayEffect(targetPS);

	APlayerCharacter* player = Cast<APlayerCharacter>(targetPS->GetPawn());

	AActor* SpawnedEffect = GetWorld()->SpawnActor<AActor>(
		BuffEffectClass,
		FVector{0,0,-95},
		FRotator::ZeroRotator
	);

	if (SpawnedEffect)
	{
		ActiveBuffEffect = SpawnedEffect;
		SpawnedEffect->AttachToActor(player, FAttachmentTransformRules::KeepRelativeTransform);
	}
}

void UYacoSkill_Q::Mul_PlayEffect_Implementation(ABattleFieldPlayerState* targetPS)
{
	
}
