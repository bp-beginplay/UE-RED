// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PickOverlay.generated.h"

UCLASS()
class RED_API UPickOverlay : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetPlayerPickWidgetArray();
	void SetCharacterButtonEnable(bool _Enable);
	void SetCountDownText(FString _CountDown);

protected:
	virtual bool Initialize() override;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void CheonIJinButtonClicked();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Test2ButtonClicked();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void Test3ButtonClicked();

private:
	UPROPERTY(meta = (BindWidget))
	class UPlayerPickWidget* RedPlayerPickWidget1;

	UPROPERTY(meta = (BindWidget))
	UPlayerPickWidget* RedPlayerPickWidget2;

	//UPROPERTY(meta = (BindWidget))
	//UPlayerPickWidget* RedPlayerPickWidget3;

	UPROPERTY(meta = (BindWidget))
	UPlayerPickWidget* BluePlayerPickWidget1;

	UPROPERTY(meta = (BindWidget))
	UPlayerPickWidget* BluePlayerPickWidget2;

	//UPROPERTY(meta = (BindWidget))
	//UPlayerPickWidget* BluePlayerPickWidget3;

	UPROPERTY()
	TArray<UPlayerPickWidget*> PlayerPickWidgetArray;

	UPROPERTY(meta = (BindWidget))
	class UButton* CheonIJinButton;

	UPROPERTY(meta = (BindWidget))
	UButton* Test3Button;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* CountDown;

public:
	TArray<UPlayerPickWidget*>GetPlayerPickWidgetArray() { return PlayerPickWidgetArray; }
};
