#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8
{
	ET_RedTeam UMETA(DisplayName = "RedTeam"),
	ET_BlueTeam UMETA(DisplayName = "BlueTeam"),
	ET_NoTeam UMETA(DisplayName = "NoTeam")
};

UENUM(BlueprintType)
enum class EMinionSpawnSlotIndex : uint8
{
	NONE UMETA(DisplayName = "None"),
	SLOT_01 UMETA(DisplayName = "Slot_1"),
	SLOT_02 UMETA(DisplayName = "Slot_2"),
	SLOT_03 UMETA(DisplayName = "Slot_3")
};

UENUM(BlueprintType)
enum class ECharacterAction : uint8
{
	Attack UMETA(DisplayName = "Attack"),
	Die UMETA(DisplayName = "Die"),
	Damaged UMETA(DisplayName = "Damaged"),

	Idle UMETA(DisplayName = "Idle")
};

UENUM(BlueprintType)
enum class EGround : uint8
{
	OnAir UMETA(DisplayName = "OnAir"),
	Ground UMETA(DisplayName = "Ground")
};

UENUM(BlueprintType)
enum class EMinionType : uint8
{
	NONE UMETA(DisplayName = "None"),
	MELEE UMETA(DisplayName = "Melee"),
	RANGE UMETA(DisplayName = "Range")
};

UENUM(BlueprintType)
enum class EBuffType : uint8
{
	NONE UMETA(DisplayName = "None"),
	REFRESHONSTACK UMETA(DisplayName = "RefreshOnStack"),
	PERSTACK UMETA(DisplayName = "PerStack"),
};