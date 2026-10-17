// Fill out your copyright notice in the Description page of Project Settings.


#include "Red_GameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemTypes.h"

#include "RED.h"

void URed_GameInstance::AddPlayer_Implementation(const FUniqueNetIdRepl& _UniqueNetID, int32 _TeamIdx)
{
	if (NetID_Team_Map.Contains(_UniqueNetID))
	{
		RemovePlayer(_UniqueNetID);
	}

	NetID_Team_Map.Add(_UniqueNetID, _TeamIdx);
}

void URed_GameInstance::RemovePlayer_Implementation(const FUniqueNetIdRepl& _UniqueNetID)
{
	if (NetID_Team_Map.Find(_UniqueNetID))
	{
		NetID_Team_Map.Remove(_UniqueNetID);
	}
}

void URed_GameInstance::SetPickCharacter_Implementation(const FUniqueNetIdRepl& _UniqueNetID, FName _PickCharacter)
{
	if (NetID_Name_Map.Contains(_UniqueNetID))
	{
		RemovePickCharacter(_UniqueNetID);
	}

	NetID_Name_Map.Add(_UniqueNetID, _PickCharacter);
}

void URed_GameInstance::RemovePickCharacter_Implementation(const FUniqueNetIdRepl& _UniqueNetID)
{
	if (NetID_Name_Map.Find(_UniqueNetID))
	{
		NetID_Name_Map.Remove(_UniqueNetID);
	}
}

FName URed_GameInstance::GetPickCharacterName_Implementation(const FUniqueNetIdRepl& _UniqueNetID)
{
	if (FName* CharacterName = NetID_Name_Map.Find(_UniqueNetID))
	{
		return *CharacterName;
	}

	return "";
}

int32 URed_GameInstance::GetLocalPlayerTeam_Implementation(const FUniqueNetIdRepl& _UniqueNetID)
{
	if (int32* teamIdx = NetID_Team_Map.Find(_UniqueNetID))
	{
		return *teamIdx;
	}

	return -1;
}

void URed_GameInstance::AddMinionSlot_Implementation(const FUniqueNetIdRepl& _UniqueNetID,
	EMinionSpawnSlotIndex _MinionSlot)
{
	if (NetID_MinionSlot_Map.Contains(_UniqueNetID))
	{
		RemoveMinionSlot(_UniqueNetID);
	}

	NetID_MinionSlot_Map.Add(_UniqueNetID, _MinionSlot);
}

void URed_GameInstance::RemoveMinionSlot_Implementation(const FUniqueNetIdRepl& _UniqueNetID)
{
	if (NetID_MinionSlot_Map.Find(_UniqueNetID))
	{
		NetID_MinionSlot_Map.Remove(_UniqueNetID);
	}
}

EMinionSpawnSlotIndex URed_GameInstance::GetMinionSlot_Implementation(const FUniqueNetIdRepl& _UniqueNetID)
{
	if (EMinionSpawnSlotIndex* SlotIndex = NetID_MinionSlot_Map.Find(_UniqueNetID))
	{
		return *SlotIndex;
	}

	return EMinionSpawnSlotIndex::NONE;
}