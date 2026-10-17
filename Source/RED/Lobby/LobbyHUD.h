// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LobbyHUD.generated.h"

UCLASS()
class RED_API ALobbyHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	void SetTeamOverlay();
	void SetPickOverlay();
	void CreatePickOverlay();
	void SetPickOverlayCountDown(int _CountDown);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(EditAnywhere)
	TSubclassOf<class UTeamOverlay> TeamOverlayClass;

	UPROPERTY()
	UTeamOverlay* TeamOverlay;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class UPickOverlay> PickOverlayClass;

	UPROPERTY()
	UPickOverlay* PickOverlay;

	UPROPERTY()
	class APlayerController* PlayerController;

public:
	UPickOverlay* GetPickOverlay() const { return PickOverlay; }
};
