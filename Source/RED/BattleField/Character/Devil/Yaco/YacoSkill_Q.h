// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Component/SkillComponent.h"
#include "YacoSkill_Q.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UYacoSkill_Q : public USkillComponent
{
	GENERATED_BODY()
	
	UYacoSkill_Q();
	
	UPROPERTY()
	class ABattleFieldPlayerController* BPC = nullptr;

public:
	virtual void SkillStart() override;
	virtual void SkillStart(AActor* _Actor);
	virtual void SkillActive() override;
	virtual void SkillCooldown() override;
	virtual void SkillCanActive() override;

	virtual void AutoRun() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetPC(class ABattleFieldPlayerController* PC);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	TSubclassOf<AActor> BuffEffectClass;

private:
	virtual void Server_Fire_Implementation() override;

	UPROPERTY()
	AActor* ActiveBuffEffect;
protected:
	float TakenHP;
	float BasicShieldAmount = 100.f;

	void RemoveShield(class ABattleFieldPlayerState* TargetPS);

	UFUNCTION(Server, Reliable)
	void Server_RequestPlayEffect(ABattleFieldPlayerState* targetPS);

	UFUNCTION(NetMulticast, Reliable)
	void Mul_PlayEffect(ABattleFieldPlayerState* targetPS);
};
