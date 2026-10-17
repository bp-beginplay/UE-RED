// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinionAIController.h"
#include "MeleeMinionAIController.generated.h"

UCLASS()
class RED_API AMeleeMinionAIController : public AMinionAIController
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

public:
	AMeleeMinionAIController();
};
