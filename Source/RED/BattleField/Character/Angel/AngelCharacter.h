// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "AngelCharacter.generated.h"

UCLASS()
class RED_API AAngelCharacter : public APlayerCharacter
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	FTimerHandle AngelPassiveHandle;

private:
	virtual void Heal();
};
