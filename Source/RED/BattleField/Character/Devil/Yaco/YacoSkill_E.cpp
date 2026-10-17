// Fill out your copyright notice in the Description page of Project Settings.


#include "YacoSkill_E.h"

#include "YacoSkill_Q.h"
#include "Kismet/GameplayStatics.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/Info/RedGamePlayTags.h"

UYacoSkill_E::UYacoSkill_E()
{
	PrimaryComponentTick.bCanEverTick = true;
	CoolTime = 5;
}

void UYacoSkill_E::Server_Fire_Implementation()
{
	Super::Server_Fire_Implementation();

	if (BPC)
	{
		ABattleFieldPlayerState* TargetPS = nullptr;
		ABattleFieldPlayerController* TargetPC = nullptr;
		if (TargetActor)
		{
			if (IActorInterface::Execute_GetETeam(TargetActor) != IActorInterface::Execute_GetETeam(GetOwner()))
			{
				return;
			}

			TargetPC = Cast<ABattleFieldPlayerController>(TargetActor->GetInstigatorController());

			if (TargetPC)
			{
				TargetPS = Cast<ABattleFieldPlayerState>(TargetPC->GetPlayerState<ABattleFieldPlayerState>());
			}

			if (TargetPS)
			{
				TakenHP = TargetPS->Health * 0.2f;
				TargetPS->Health *= 0.8f;

				BPC->GetPlayerState<ABattleFieldPlayerState>()->HealHealth(TakenHP);

				// 타겟에게 흡혈, 이속 공속 공격력 버프 부여
				// 게임태그를 통해 부여할 예정

				FBuffData AttackBuff;
				AttackBuff.Duration = BuffDuration;
				AttackBuff.BuffTag = FRedGameplayTags::Buff_Yaco_E_Attack_Add;
				AttackBuff.Value = AttackBuffValue;
				AttackBuff.BuffType = EBuffType::REFRESHONSTACK;
				AttackBuff.MaxCount = 1;
				AttackBuff.Contribute = [Value = AttackBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
					{
						Add.Attack += Value * BuffAmount;
					};

				FBuffData AttackSpeedBuff;
				AttackSpeedBuff.Duration = BuffDuration;
				AttackSpeedBuff.BuffTag = FRedGameplayTags::Buff_Yaco_E_AttackSpeed_Mul;
				AttackSpeedBuff.Value = AttackSpeedBuffValue;
				AttackSpeedBuff.BuffType = EBuffType::REFRESHONSTACK;
				AttackSpeedBuff.MaxCount = 1;
				AttackSpeedBuff.Contribute = [Value = AttackSpeedBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
					{
						Mul.AttackSpeed += Value * BuffAmount;
					};

				FBuffData SpeedBuff;
				SpeedBuff.Duration = BuffDuration;
				SpeedBuff.BuffTag = FRedGameplayTags::Buff_Yaco_E_Speed_Add;
				SpeedBuff.Value = SpeedBuffValue;
				SpeedBuff.BuffType = EBuffType::REFRESHONSTACK;
				SpeedBuff.MaxCount = 1;
				SpeedBuff.Contribute = [Value = SpeedBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
					{
						Add.Speed += Value * BuffAmount;
					};

				FBuffData LifeSteelBuff;
				LifeSteelBuff.Duration = BuffDuration;
				LifeSteelBuff.BuffTag = FRedGameplayTags::Buff_Yaco_E_LifeSteel_Add;
				LifeSteelBuff.Value = LifeSteelBuffValue;
				LifeSteelBuff.BuffType = EBuffType::REFRESHONSTACK;
				LifeSteelBuff.MaxCount = 1;
				LifeSteelBuff.Contribute = [Value = LifeSteelBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
					{
						Add.LifeSteel += Value * BuffAmount;
					};

				TargetPS->AddBuffs(AttackBuff, AttackSpeedBuff, SpeedBuff, LifeSteelBuff);

				TargetPS->OnRep_Shield();
				TargetPS->OnRep_Health();

				Server_RequestPlayEffect(TargetPS);

				Server_ChangeSkillState(ESkillState::Cooldown);

				Client_SetSkillIconOpacity(false);
				Client_UpdateCoolTimeTextBlock(CoolTime);
				Client_ChangeSkillState(ESkillState::CanActive, CoolTime);
			}
		}
	}
}

void UYacoSkill_E::AutoRun()
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

void UYacoSkill_E::SkillStart()
{
	Super::SkillStart();
}

void UYacoSkill_E::SkillStart(AActor* _Actor)
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

void UYacoSkill_E::SkillActive()
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

void UYacoSkill_E::SkillCooldown()
{
	Super::SkillCooldown();
}

void UYacoSkill_E::SkillCanActive()
{
	Super::SkillCanActive();

	Client_SetSkillIconOpacity(true);
}

void UYacoSkill_E::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AutoRun();
}

void UYacoSkill_E::SetPC(class ABattleFieldPlayerController* PC)
{
	BPC = PC;
}

void UYacoSkill_E::Server_RequestPlayEffect_Implementation(ABattleFieldPlayerState* targetPS)
{
	Mul_PlayEffect(targetPS);
}

void UYacoSkill_E::Mul_PlayEffect_Implementation(ABattleFieldPlayerState* targetPS)
{
	APlayerCharacter* player = Cast<APlayerCharacter>(targetPS->GetPawn());

	UGameplayStatics::SpawnEmitterAttached(
		ParticleSystem,
		player->GetMesh(),   // 붙일 컴포넌트
		TEXT("FX_Root"),                         // 소켓 이름 (없으면 None)
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		FVector(0.5f),
		EAttachLocation::KeepRelativeOffset,
		true                               // AutoDestroy
	);
}