// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "RED/Info/EnumInfo.h"
#include "LobbyPlayerState.generated.h"

UCLASS()
class RED_API ALobbyPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Server, Reliable)
	void Server_SetTeam(ETeam _Team);

	UFUNCTION(Server, Reliable)
	void Server_SetCharacterName(FName _Name);

	UFUNCTION(NetMulticast, Reliable)
	void Multi_SetPickCharacter(const FUniqueNetIdRepl& _UniqueNetID, FName _Name);

protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

private:
	UFUNCTION()
	void OnRep_SetTeam();

	UFUNCTION()
	void OnRep_SetCharacterName();

	virtual void CopyProperties(class APlayerState* PlayerState) override;

private:
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SetTeam, meta = (AllowPrivateAccess = "true"))
	ETeam Team = ETeam::ET_NoTeam;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EMinionSpawnSlotIndex OwnedSlot = EMinionSpawnSlotIndex::SLOT_01;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_SetCharacterName, meta = (AllowPrivateAccess = "true"))
	FName PickCharacter = "None";

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	int PickSequence;

public:
	ETeam GetTeam() const { return Team; }
	FName GetCharacterName() const { return PickCharacter; }
	void SetPickSequence(int _Sequence) { PickSequence = _Sequence; }
	int GetPickSequence() const { return PickSequence; }
	void SetMinionSlot(EMinionSpawnSlotIndex _SlotIndex);
};
