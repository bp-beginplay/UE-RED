// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/// <summary>
/// 나중에 버프 종류별이 아닌 모든 버프마다 태그를 구분하여 쓸 예정
/// 
/// </summary>

struct FRedGameplayTags
{
	static FGameplayTag Buff_Yaco_Q_Shield;

	static FGameplayTag Buff_Yaco_E_Attack_Add;
	static FGameplayTag Buff_Yaco_E_AttackSpeed_Mul;
	static FGameplayTag Buff_Yaco_E_Speed_Add;
	static FGameplayTag Buff_Yaco_E_LifeSteel_Add;

	static FGameplayTag Buff_Yaco_R_Attack_Add;
	static FGameplayTag Buff_Yaco_R_AttackSpeed_Mul;
	static FGameplayTag Buff_Yaco_R_Speed_Add;
	static FGameplayTag Buff_Yaco_R_Defence_Add;

	static FGameplayTag Buff_CheonIJin_E_Stun;


	static void InitializeTags();
};
