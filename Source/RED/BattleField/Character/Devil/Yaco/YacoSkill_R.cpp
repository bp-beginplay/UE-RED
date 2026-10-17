// Fill out your copyright notice in the Description page of Project Settings.


#include "YacoSkill_R.h"

#include "Kismet/GameplayStatics.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/Info/RedGamePlayTags.h"

UYacoSkill_R::UYacoSkill_R()
{
	PrimaryComponentTick.bCanEverTick = true;
	CoolTime = 20;
}

void UYacoSkill_R::SkillStart()
{
	Super::SkillStart();
}

void UYacoSkill_R::SkillActive()
{
	Super::SkillActive();

	if (BPC)
	{
		Server_RequestPlayEffect(BPC->GetPlayerState<ABattleFieldPlayerState>());
	}
	PlayAnimMontage(SkillActiveMontage);
}

void UYacoSkill_R::SkillCooldown()
{
	Super::SkillCooldown();
}

void UYacoSkill_R::SkillCanActive()
{
	Super::SkillCanActive();

	Client_SetSkillIconOpacity(true);
}

void UYacoSkill_R::SetPC(class ABattleFieldPlayerController* PC)
{
	BPC = PC;
}

void UYacoSkill_R::Server_Fire_Implementation()
{
	Super::Server_Fire_Implementation();

	if (BPC)
	{
		ABattleFieldPlayerState* TargetPS = nullptr;

		TargetPS = BPC->GetPlayerState<ABattleFieldPlayerState>();

		if (TargetPS)
		{
			FBuffData AttackBuff;
			AttackBuff.Duration = BuffDuration;
			AttackBuff.BuffTag = FRedGameplayTags::Buff_Yaco_R_Attack_Add;
			AttackBuff.Value = AttackBuffValue;
			AttackBuff.BuffType = EBuffType::REFRESHONSTACK;
			AttackBuff.MaxCount = 1;
			AttackBuff.Contribute = [Value = AttackBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
				{
					Add.Attack += Value * BuffAmount;
				};

			FBuffData AttackSpeedBuff;
			AttackSpeedBuff.Duration = BuffDuration;
			AttackSpeedBuff.BuffTag = FRedGameplayTags::Buff_Yaco_R_AttackSpeed_Mul;
			AttackSpeedBuff.Value = AttackSpeedBuffValue;
			AttackSpeedBuff.BuffType = EBuffType::REFRESHONSTACK;
			AttackSpeedBuff.MaxCount = 1;
			AttackSpeedBuff.Contribute = [Value = AttackSpeedBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
				{
					Mul.AttackSpeed += Value * BuffAmount;
				};

			FBuffData SpeedBuff;
			SpeedBuff.Duration = BuffDuration;
			SpeedBuff.BuffTag = FRedGameplayTags::Buff_Yaco_R_Speed_Add;
			SpeedBuff.Value = SpeedBuffValue;
			SpeedBuff.BuffType = EBuffType::REFRESHONSTACK;
			SpeedBuff.MaxCount = 1;
			SpeedBuff.Contribute = [Value = SpeedBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
				{
					Add.Speed += Value * BuffAmount;
				};

			FBuffData DefenceBuff;
			DefenceBuff.Duration = BuffDuration;
			DefenceBuff.BuffTag = FRedGameplayTags::Buff_Yaco_R_Defence_Add;
			DefenceBuff.Value = DefenceBuffValue;
			DefenceBuff.BuffType = EBuffType::REFRESHONSTACK;
			DefenceBuff.MaxCount = 1;
			DefenceBuff.Contribute = [Value = DefenceBuff.Value](FStatValues& Add, FStatValues& Mul, int32 BuffAmount)
				{
					Add.LifeSteel += Value * BuffAmount;
				};

			TargetPS->AddBuffs(AttackBuff, AttackSpeedBuff, SpeedBuff, DefenceBuff);

			TargetPS->OnRep_Shield();
			TargetPS->OnRep_Health();

			Server_ChangeSkillState(ESkillState::Cooldown);

			Client_SetSkillIconOpacity(false);
			Client_UpdateCoolTimeTextBlock(CoolTime);
			Client_ChangeSkillState(ESkillState::CanActive, CoolTime);
		}
	}
}

void UYacoSkill_R::Server_RequestPlayEffect_Implementation(ABattleFieldPlayerState* targetPS)
{
	Mul_PlayEffect(targetPS);
}

void UYacoSkill_R::Mul_PlayEffect_Implementation(ABattleFieldPlayerState* targetPS)
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
