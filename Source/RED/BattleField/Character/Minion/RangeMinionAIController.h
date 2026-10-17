// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MinionAIController.h"
#include "RangeMinionAIController.generated.h"

UCLASS()
class RED_API ARangeMinionAIController : public AMinionAIController
{
	GENERATED_BODY()
	
	virtual void BeginPlay() override;

public:
	ARangeMinionAIController();
};