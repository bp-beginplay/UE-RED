#pragma once
#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "EnumInfo.h"
#include "StructInfo.generated.h"


USTRUCT(BlueprintType)
struct FStatValues
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float Attack = 0.f;
	float Defense = 0.f;
	float AttackSpeed = 1.f;
	float Speed = 0.f;
	float LifeSteel = 0.f;
};

USTRUCT(BlueprintType)
struct FShieldData
{
	GENERATED_BODY()

	UPROPERTY()
	float ShieldAmount = 0.f;

	UPROPERTY()
	float Duration = 0.f;

	UPROPERTY()
	float StartTime = 0.f;

	FTimerHandle TimerHandle;

	UPROPERTY()
	TSubclassOf<class USkillComponent> FromSkill; // 어떤 스킬에서 왔는지

	UPROPERTY()
	AActor* SpawnedEffect;
};

USTRUCT()
struct FBuffData
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag BuffTag = FGameplayTag::EmptyTag;

	// 지속시간이 0이면 영구효과
	UPROPERTY()
	float Duration = 0.f;

	UPROPERTY()
	float Value = 0.f;

	UPROPERTY()
	FTimerHandle TimerHandle;

	UPROPERTY()
	EBuffType BuffType = EBuffType::NONE;

	UPROPERTY()
	int32 MaxCount = 1;

	// 버프 종류가 합적용인지 곱적용인지 + 버프가 중첩가능하다면 버프의 개수가 몇개인지
	TFunction<void(FStatValues& Add, FStatValues& Mul, int32 BuffAmount)> Contribute = [](FStatValues&, FStatValues&, int32) {};
};