// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Component/SkillComponent.h"
#include "CheonIJinSkill_W.generated.h"

UCLASS()
class RED_API UCheonIJinSkill_W : public USkillComponent
{
	GENERATED_BODY()

public:
	UCheonIJinSkill_W();

protected:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void AutoRun() override;

public:
	virtual void SkillSetTarget() override;
	virtual void SkillStart(FVector _TargetLocation) override;
	virtual void SkillActive() override;
	virtual void SkillCanActive() override;
	virtual void PlayAnimMontage(UAnimMontage* _Montage) override;
private:
	virtual void Server_Fire_Implementation() override;
};