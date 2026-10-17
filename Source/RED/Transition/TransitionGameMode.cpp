// Fill out your copyright notice in the Description page of Project Settings.


#include "TransitionGameMode.h"

void ATransitionGameMode::BeginPlay()
{
	Super::BeginPlay();

	GetWorldTimerManager().SetTimer(
	WaitTimer,
	[this]() // this와 매개변수를 캡처
	{
		CanTravel = true;
	},
	5.0f,
	false
	);
}

bool ATransitionGameMode::CanServerTravel(const FString& URL, bool bAbsolute)
{
	bool bParentCanTravel = Super::CanServerTravel(URL, bAbsolute);

	return bParentCanTravel && CanTravel;
}