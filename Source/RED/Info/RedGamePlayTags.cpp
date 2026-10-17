// Fill out your copyright notice in the Description page of Project Settings.


#include "RedGamePlayTags.h"

FGameplayTag FRedGameplayTags::Buff_Yaco_Q_Shield;

FGameplayTag FRedGameplayTags::Buff_Yaco_E_Attack_Add;
FGameplayTag FRedGameplayTags::Buff_Yaco_E_AttackSpeed_Mul;
FGameplayTag FRedGameplayTags::Buff_Yaco_E_Speed_Add;
FGameplayTag FRedGameplayTags::Buff_Yaco_E_LifeSteel_Add;

FGameplayTag FRedGameplayTags::Buff_Yaco_R_Attack_Add;
FGameplayTag FRedGameplayTags::Buff_Yaco_R_AttackSpeed_Mul;
FGameplayTag FRedGameplayTags::Buff_Yaco_R_Speed_Add;
FGameplayTag FRedGameplayTags::Buff_Yaco_R_Defence_Add;

FGameplayTag FRedGameplayTags::Buff_CheonIJin_E_Stun;

void FRedGameplayTags::InitializeTags()
{
	Buff_Yaco_Q_Shield = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_Q.Shield"));

	Buff_Yaco_E_Attack_Add = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.Attack.Add"));
	Buff_Yaco_E_AttackSpeed_Mul = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.AttackSpeed.Mul"));
	Buff_Yaco_E_Speed_Add = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.Speed.Add"));
	Buff_Yaco_E_LifeSteel_Add = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.LifeSteel.Add"));

	Buff_Yaco_R_Attack_Add = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.LifeSteel.Add"));
	Buff_Yaco_R_AttackSpeed_Mul = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.LifeSteel.Add"));
	Buff_Yaco_R_Speed_Add = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.LifeSteel.Add"));
	Buff_Yaco_R_Defence_Add = FGameplayTag::RequestGameplayTag(FName("Buff.Yaco_E.LifeSteel.Add"));

	Buff_CheonIJin_E_Stun = FGameplayTag::RequestGameplayTag(FName("DeBuff.CheonIJin_E.Stun"));
}