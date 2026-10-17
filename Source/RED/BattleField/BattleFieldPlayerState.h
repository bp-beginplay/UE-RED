// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "../Interface/ActorInterface.h"
#include "../Info/EnumInfo.h"
#include "../Info/StructInfo.h"

#include "BattleFieldPlayerState.generated.h"

// OverflowHp: MaxHp 초과분
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHpChanged, float /*OverflowHp*/);

UCLASS()
class RED_API ABattleFieldPlayerState : public APlayerState, public IActorInterface
{
	GENERATED_BODY()
	
// 김동호 코드
public:
	ABattleFieldPlayerState();

	UFUNCTION(BlueprintCallable)
	void CalculateHealth(float Damage);

	void HealHealth(float HealAmount);

	UFUNCTION(Server, Reliable)
	void Server_UpgradeMaxHealth(float _MaxHealth);
	UFUNCTION(Server, Reliable)
	void Server_AddPoint(float _Point);
	UFUNCTION(Server, Reliable)
	void Server_AddBaseState(float _Attack);
protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

private:
	virtual void CopyProperties(class APlayerState* PlayerState) override;

public:
	UFUNCTION()
	void OnRep_Health();
	UFUNCTION()
	void OnRep_Mana();
	UFUNCTION()
	void OnRep_Shield();
	UFUNCTION()
	void OnRep_Point();

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> CharacterDataTable;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_Health, meta = (AllowPrivateAccess = "true"))
	float MaxHealth;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Mana, meta = (AllowPrivateAccess = "true"))
	float MaxMana;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, ReplicatedUsing = OnRep_Health, meta = (AllowPrivateAccess = "true"))
	float Health;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Mana, meta = (AllowPrivateAccess = "true"))
	float Mana;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Shield, meta = (AllowPrivateAccess = "true"))
	TArray<FShieldData> ActiveShields;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Shield, meta = (AllowPrivateAccess = "true"))
	float TotalShield;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_Point, meta = (AllowPrivateAccess = "true"))
	float Point = 10.f;

	//UPROPERTY(EditAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	//float AttackRange;

	UPROPERTY(EditAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	FStatValues BaseStats;

	UPROPERTY(EditAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	FStatValues TotalStats;


public:
	UPROPERTY(EditAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	ETeam Team = ETeam::ET_NoTeam;

	UPROPERTY(EditAnywhere, Replicated, meta = (AllowPrivateAccess = "true"))
	FName CharacterName = "CheonIJin";

public:
	UFUNCTION(BlueprintCallable)
	void SetTeam(ETeam _Team) { Team = _Team; }

	UFUNCTION(BlueprintCallable)
	FName GetPickCharacter() const { return CharacterName; }

	UFUNCTION(BlueprintCallable)
	void State_SetPickCharacter(FName _Name) { CharacterName = _Name; }

// 영빈 코드
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	EMinionSpawnSlotIndex OwnedSlot = EMinionSpawnSlotIndex::SLOT_01;

	UPROPERTY()
	TArray<class AControlPointBaseActor*> ControlPointList;

	void SetSpawnSlot();
protected:
	// 그냥 들어온 모든 버프 관리용
	UPROPERTY()
	TArray<FBuffData> ActiveBuffs;

	UPROPERTY()
	TArray<FBuffData> ActiveDeBuffs;

	// 만약 ui에 버프 띄우게 된다면 스택등 표시용
	UPROPERTY()
	TMap<FGameplayTag, int32> TagCount;

	UFUNCTION()
	void AddBuff(const FBuffData& Buff);

	UFUNCTION()
	void AddDeBuff(const FBuffData& Buff);

	void ActiveDeBuff();
	void RemoveDeBuff(const FBuffData& Buff);
public:
	// 이걸로 최종 스탯 갱신도 가능
	void CalculateBuff();

	FOnHpChanged OnHpChanged;

	virtual int32 GetTeam_Implementation() const override
	{
		return static_cast<int32>(Team);
	}

	virtual ETeam GetETeam_Implementation() const override
	{
		return Team;
	}

	UFUNCTION(BlueprintCallable)
	void SetTeam_int(int32 _TeamIdx);

	UFUNCTION()
	void SetMinionMovePoint(int32 _Value);

	UFUNCTION()
	void RemoveBuff(FGameplayTag BuffTag);

	void ClearBuff();

	template<typename... T>
	void AddBuffs(T... Buffs)
	{
		(AddBuff(Buffs), ...);
		// 이후 버프 순회하며 버프주기
		CalculateBuff();
	}

	template<typename... T>
	void AddDeBuffs(T... Buffs)
	{
		(AddDeBuff(Buffs), ...);
		// 이후 버프 순회하며 버프주기
		CalculateBuff();
	}
};
