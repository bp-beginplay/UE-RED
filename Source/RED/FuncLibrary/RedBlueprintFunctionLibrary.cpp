// Fill out your copyright notice in the Description page of Project Settings.


#include "RedBlueprintFunctionLibrary.h"

float URedBlueprintFunctionLibrary::CalculateDamage(float _Atk, float _Def)
{
	float Damage = FMath::Max(_Atk - _Def, 1);
	return Damage;
}

bool URedBlueprintFunctionLibrary::IsDiedTarget(AActor* _Target)
{
	return true;
}