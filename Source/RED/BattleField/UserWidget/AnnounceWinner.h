// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnnounceWinner.generated.h"

/**
 * 
 */
UCLASS()
class RED_API UAnnounceWinner : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* WinnerTextBlock;
};
