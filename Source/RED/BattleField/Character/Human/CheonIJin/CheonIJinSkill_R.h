// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Component/SkillComponent.h"
#include "CheonIJinSkill_R.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UCheonIJinSkill_R : public USkillComponent
{
	GENERATED_BODY()
	
public:
	virtual void SkillSetTarget() override;
	virtual void SkillStart(FVector _TargetLocation) override;
	virtual void SkillActive() override;
	virtual void PlayAnimMontage(UAnimMontage* _Montage) override;
	virtual void Server_Fire_Implementation() override;
	virtual void SkillCooldown() override;
	virtual void SkillCanActive() override;
};
