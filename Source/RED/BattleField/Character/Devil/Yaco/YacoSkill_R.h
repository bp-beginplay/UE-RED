// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Red/BattleField/Component/SkillComponent.h"
#include "YacoSkill_R.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UYacoSkill_R : public USkillComponent
{
	GENERATED_BODY()
	
	UYacoSkill_R();

	UPROPERTY()
	class ABattleFieldPlayerController* BPC = nullptr;

	float BuffDuration = 10.f;

	float AttackBuffValue = 40.f;
	float DefenceBuffValue = 40.f;
	float AttackSpeedBuffValue = 0.5f;
	float SpeedBuffValue = 20.f;

public:
	virtual void SkillStart() override;
	virtual void SkillActive() override;
	virtual void SkillCooldown() override;
	virtual void SkillCanActive() override;

	void SetPC(class ABattleFieldPlayerController* PC);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* ParticleSystem;

private:
	virtual void Server_Fire_Implementation() override;

	UFUNCTION(Server, Reliable)
	void Server_RequestPlayEffect(ABattleFieldPlayerState* targetPS);

	UFUNCTION(NetMulticast, Reliable)
	void Mul_PlayEffect(ABattleFieldPlayerState* targetPS);
};
