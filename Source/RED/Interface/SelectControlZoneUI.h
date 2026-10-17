// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RED/Info/EnumInfo.h"
#include "SelectControlZoneUI.generated.h"

UCLASS()
class RED_API USelectControlZoneUI : public UUserWidget
{
	GENERATED_BODY()

protected:

	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* Zone1;

	UPROPERTY(meta = (BindWidget))
	class UButton* Zone2;

	UPROPERTY(meta = (BindWidget))
	class UButton* Zone3;

	UPROPERTY(meta = (BindWidget))
	class UButton* Zone4;

	UPROPERTY(meta = (BindWidget))
	class UButton* Zone5;

	UPROPERTY(meta = (BindWidget))
	class UButton* Zone6;

	UPROPERTY(meta = (BindWidget))
	class UButton* Zone7;

	UFUNCTION()
	void SetButtonColor(UButton* Button, ETeam Team);

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetOwner(APlayerController* Controller);

	UFUNCTION()
	void ChangeButtonColor(int32 ButtonIdx, ETeam Team);


	// 김동호
public:
	UPROPERTY(meta = (BindWidget))
	UButton* HealthUpgradeButton;

	UPROPERTY(meta = (BindWidget))
	UButton* AttackUpgradeButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	class UTextBlock* RemainPointTextBlock;

	virtual bool Initialize() override;

	UFUNCTION()
	void UpgradeHealth();
	UFUNCTION()
	void UpgradeAttack();


};
