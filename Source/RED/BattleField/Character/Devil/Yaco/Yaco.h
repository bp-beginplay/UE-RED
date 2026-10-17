// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../DevilCharacter.h"
#include "Yaco.generated.h"

/**
 * 
 */
UCLASS()
class RED_API AYaco : public ADevilCharacter
{
	GENERATED_BODY()

	AYaco();
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

protected:
	UFUNCTION()
	void PassiveSkill(float OverFlowHP);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UYacoSkill_Q* Q_SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UYacoSkill_W* W_SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UYacoSkill_E* E_SkillComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UYacoSkill_R* R_SkillComponent;

	UPROPERTY()
	class ABattleFieldGameState* CachedGameState = nullptr;

	float PassiveRange = 1000;

public:
	virtual void Q_Skill() override;
	virtual void W_Skill() override;
	virtual void E_Skill() override;
	virtual void R_Skill() override;
	virtual void LMB_Pressed() override;
	virtual void RMB_Pressed() override;
	virtual void LMB_Released(FVector _TargetPosition, AActor* _Actor) override;
	virtual void RMB_Released() override;

	virtual void PossessedBy(AController* NewController) override;
};