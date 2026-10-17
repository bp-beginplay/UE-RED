// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Info/EnumInfo.h"
#include "Engine/GameInstance.h"
#include "Red_GameInstance.generated.h"

/**
 * 
 */
UCLASS()
class RED_API URed_GameInstance : public UGameInstance
{
	GENERATED_BODY()
private:
	UPROPERTY(BlueprintReadWrite , EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<FUniqueNetIdRepl, int32> NetID_Team_Map;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<FUniqueNetIdRepl, FName> NetID_Name_Map;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TMap<FUniqueNetIdRepl, EMinionSpawnSlotIndex> NetID_MinionSlot_Map;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int32 TeamIdx;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	void AddPlayer(const FUniqueNetIdRepl& _UniqueNetID ,int32 _TeamIdx);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	void RemovePlayer(const FUniqueNetIdRepl& _UniqueNetID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	void SetPickCharacter(const FUniqueNetIdRepl& _UniqueNetID, FName _PickCharacter);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	void RemovePickCharacter(const FUniqueNetIdRepl& _UniqueNetID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	FName GetPickCharacterName(const FUniqueNetIdRepl& _UniqueNetID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	int32 GetLocalPlayerTeam(const FUniqueNetIdRepl& _UniqueNetID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	void AddMinionSlot(const FUniqueNetIdRepl& _UniqueNetID, EMinionSpawnSlotIndex _MinionSlot);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	void RemoveMinionSlot(const FUniqueNetIdRepl& _UniqueNetID);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "GameInstance")
	EMinionSpawnSlotIndex GetMinionSlot(const FUniqueNetIdRepl& _UniqueNetID);
};
