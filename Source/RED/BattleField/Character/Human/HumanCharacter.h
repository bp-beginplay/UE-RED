// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "HumanCharacter.generated.h"

UCLASS()
class RED_API AHumanCharacter : public APlayerCharacter
{
	GENERATED_BODY()

protected:
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_PlayHealMontage();

private:
	UPROPERTY(EditAnywhere)
	class UAnimMontage* HealMontage;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* HealEffect;

	float ReadyToHealTime = 3.0f;

};
