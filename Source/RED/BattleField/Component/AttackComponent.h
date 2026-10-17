// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SkillComponent.h"
#include "AttackComponent.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UAttackComponent : public USkillComponent
{
	GENERATED_BODY()
	
public:
	UAttackComponent();

	virtual void SkillSetTarget() override;
	virtual void SkillStart(FVector _TargetLocation) override;
	virtual void SkillActive() override;
	void NearistTargetActor();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void AutoRun() override;
	virtual void EndAnimMontage() override;
	virtual void PlayAnimMontage(UAnimMontage* _Montage) override;
private:
	virtual void Server_Fire_Implementation() override;
};
