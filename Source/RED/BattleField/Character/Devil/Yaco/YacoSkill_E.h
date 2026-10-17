// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Red/BattleField/Component/SkillComponent.h"
#include "YacoSkill_E.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UYacoSkill_E : public USkillComponent
{
	GENERATED_BODY()
	
	UYacoSkill_E();

	UPROPERTY()
	class ABattleFieldPlayerController* BPC = nullptr;

	float TakenHP;

	float BuffDuration = 5.f;

	float AttackBuffValue = 20.f;
	float AttackSpeedBuffValue = 0.2f;
	float SpeedBuffValue = 10.f;
	float LifeSteelBuffValue = 0.2f;

	virtual void Server_Fire_Implementation() override;

	virtual void AutoRun() override;
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* ParticleSystem;

	virtual void SkillStart() override;
	virtual void SkillStart(AActor* _Actor) override;
	virtual void SkillActive() override;
	virtual void SkillCooldown() override;
	virtual void SkillCanActive() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetPC(class ABattleFieldPlayerController* PC);

	UFUNCTION(Server, Reliable)
	void Server_RequestPlayEffect(ABattleFieldPlayerState* targetPS);

	UFUNCTION(NetMulticast, Reliable)
	void Mul_PlayEffect(ABattleFieldPlayerState* targetPS);
};
