// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RedBlueprintFunctionLibrary.generated.h"

UCLASS()
class RED_API URedBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "GameLogic|Damage")
	static float CalculateDamage(float _Atk, float _Def);

	UFUNCTION(BlueprintCallable, Category = "GameLogic|Utility")
	static bool IsDiedTarget(AActor* _Target);
};
