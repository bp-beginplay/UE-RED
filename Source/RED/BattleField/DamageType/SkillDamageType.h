// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "SkillDamageType.generated.h"

/**
 * 
 */
UCLASS()
class RED_API USkillDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:

	UPROPERTY()
	UActorComponent* SkillComponent;
};
