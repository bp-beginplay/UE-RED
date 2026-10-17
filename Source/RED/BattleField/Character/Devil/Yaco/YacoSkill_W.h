// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Red/BattleField/Component/SkillComponent.h"
#include "YacoSkill_W.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UYacoSkill_W : public USkillComponent
{
	GENERATED_BODY()
	
	UYacoSkill_W();

	float Radius = 1000.f;
	float Damage = 10.f;

	virtual void Server_Fire_Implementation() override;
public:
	virtual void SkillStart() override;
	virtual void SkillStart(FVector _TargetLocation) override;
	virtual void SkillActive() override;
	virtual void SkillCooldown() override;
	virtual void SkillCanActive() override;
	virtual void AutoRun() override;

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(Server, Reliable)
	void Server_SpawnSkill(FVector CenterLocation);

	void ApplyDamageNDestroy(class ASkillRangeActor* SkillRangeActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "VFX")
	UParticleSystem* ParticleSystem;

	UFUNCTION(Server,Reliable)
	void Server_RequestPlayEffect(ASkillRangeActor* SkillRangeActor);

	UFUNCTION(NetMulticast, Reliable)
	void Mul_PlayEffect(ASkillRangeActor* SkillRangeActor);
};
