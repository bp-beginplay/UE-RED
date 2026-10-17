// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "TransitionGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RED_API ATransitionGameMode : public AGameMode
{
	GENERATED_BODY()
	virtual void BeginPlay() override;
	virtual bool CanServerTravel(const FString& URL, bool bAbsolute) override;

	bool CanTravel = false;
	FTimerHandle WaitTimer;
};
