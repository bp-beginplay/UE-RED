// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Component/SkillComponent.h"
#include "CheonIJinSkill_Q.generated.h"

UCLASS()
class RED_API UCheonIJinSkill_Q : public USkillComponent
{
	GENERATED_BODY()
	
public:
	void FlyJump();

	virtual void SkillCanActive() override;
	virtual void SkillStart() override;
	virtual void SkillActive() override;




	virtual void SkillCooldown() override;




	FTimerHandle FlyJumpTHandle;
};
