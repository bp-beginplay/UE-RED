// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerOverlay.generated.h"

UCLASS()
class RED_API UPlayerOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UCharacterInfo* CharacterInfo;

	UPROPERTY(meta = (BindWidget))
	class UTeamScore* TeamScore;

	UPROPERTY(meta = (BindWidget))
	class UImage* FadeOutImage;

	UPROPERTY(meta = (BindWidget))
	class UImage* MiniMap;

	UPROPERTY(meta = (BindWidget))
	class UOverlay* LoadingOverlay;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	UMaterialInterface* MiniMapBaseMaterial;

	UFUNCTION()
	void CreateUIMat(UTextureRenderTarget2D* RT);

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDoubleClick(
		const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void ForceLMBRelease();

private:
	UPROPERTY()
	class APlayerCharacter* Player;

	UPROPERTY()
	class ABattleFieldPlayerController* BPC;

public:
	bool IsLMB_HoldingMap = false;
	bool IsLMB_MovingMap = false;
	bool IsRMB_HoldingMap = false;

	FGeometry Geo;
	FVector2D Center;
	FVector2D MapPos{ 15000,-15000 };
	float Radius;
	float Ratio;
	float MapSize = 30000.f;
};
