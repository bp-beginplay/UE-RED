// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AngelCharacter.h"
#include "Temp.generated.h"

/**
 * 
 */
UCLASS()
class RED_API ATemp : public AAngelCharacter
{
	GENERATED_BODY()
	
public:
	ATemp();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void Heal() override;

	float SavedHealAmount;
	float PassiveRange = 1000;

	UPROPERTY()
	class ABattleFieldGameState* CachedGameState = nullptr;
};
