// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "MinionAIController.generated.h"

UCLASS()
class RED_API AMinionAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMinionAIController();

protected:
	UFUNCTION()
	virtual void OnPossess(APawn* InPawn) override;

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	const TArray<class AControlPointBaseActor*>* ControlPointList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	AActor* Target = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	AActor* LastTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	AActor* TargetCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	TArray<AActor*> TargetList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	int32 TargetControlPointValue = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minion")
	class ABaseMinionCharacter* OwnedMinion;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "PlayerState")
	class AMinionSpawnSlotBaseActor* SpawnSlot;

	UPROPERTY()
	FTimerHandle CheckDistHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bool")
	bool IsAttacking = false;
public:

	void BeginOverlap(AActor* _Actor);

	void EndOverlap(AActor* _Actor);

	UFUNCTION()
	void OnDestroyed(AActor* _Actor);

	void FindNewTarget(int32 _Value);

	void Died();

protected:
	AActor* FindClosestTarget();

	AActor* SetTargetControlZone(int32 _Value);

	UFUNCTION()
	bool IsEnemy(AActor* _OtherActor) const;

	UPROPERTY()
	bool IsDied = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float AttackRange = 100.f;

	UPROPERTY(BlueprintReadOnly)
	float AttackRangeSq;

	UFUNCTION()
	void CalDistToTarget();

	UFUNCTION(BlueprintCallable)
	void Controller_ApplyDamage(float Damage);
};
