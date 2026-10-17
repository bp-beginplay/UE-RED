// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entity/FOW_VisibilityEntity_Interface.h"
#include "RED/Info/EnumInfo.h"
#include "UObject/Interface.h"
#include "ActorInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UActorInterface : public UFOW_VisibilityEntity_Interface
{
	GENERATED_BODY()
};

class RED_API IActorInterface: public IFOW_VisibilityEntity_Interface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Getter")
	int32 GetTeam() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Getter")
	ETeam GetETeam() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Getter")
	bool GetIsDied() const;
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Getter")
	int32 GetVisibility() const;

	UFUNCTION(BlueprintNativeEvent)
	void HighlightActor();
	UFUNCTION(BlueprintNativeEvent)
	void UnHighlightActor();
};
