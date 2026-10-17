// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Character/Human/HumanCharacter.h"
#include "CheonIJin.generated.h"

UCLASS()
class RED_API ACheonIJin : public AHumanCharacter
{
	GENERATED_BODY()
	
public:
	ACheonIJin();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCheonIJinSkill_Q* Q_SkillComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCheonIJinSkill_W* W_SkillComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCheonIJinSkill_E* E_SkillComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UCheonIJinSkill_R* R_SkillComponent;
	virtual void BeginPlay() override;

public:
	virtual void Q_Skill() override;
	virtual void W_Skill() override;
	virtual void E_Skill() override;
	virtual void R_Skill() override;
	virtual void LMB_Released(FVector _TargetPosition, AActor* _Actor) override;
	virtual void RMB_Pressed() override;
};
