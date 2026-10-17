// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RED/Info/EnumInfo.h"
#include "MinionSpawnSlotBaseActor.generated.h"

UCLASS()
class RED_API AMinionSpawnSlotBaseActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AMinionSpawnSlotBaseActor();

	FTimerHandle FirstSpawnTimerHandle;

	FTimerHandle SecondSpawnTimerHandle;

	FTimerHandle ThirdSpawnTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minion")
	TArray<class ABaseMinionCharacter*> OwnedMinionList;

	UPROPERTY(EditAnywhere, Category = "Minion")
	TSubclassOf<class ABaseMinionCharacter> SpawnMeleeMinionClass;

	UPROPERTY(EditAnywhere, Category = "Minion")
	TSubclassOf<class ABaseMinionCharacter> SpawnRangeMinionClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "ControlPoint")
	class AControlPointBaseActor* OwnerControlPoint;

	UFUNCTION()
	void SetOwnerControlPoint(class AControlPointBaseActor* _ControlPointBaseActor)
	{
		OwnerControlPoint = _ControlPointBaseActor;
	}

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION(Server, Reliable)
	void Server_SpawnMinionCycle();

	UFUNCTION()
	void SpawnMinionTeam();

	UFUNCTION()
	void SpawnMinion(TSubclassOf<class ABaseMinionCharacter> _Class);

	UFUNCTION()
	void MinionOnDestroyed(AActor* DestroyedActor);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EMinionSpawnSlotIndex SlotIndex = EMinionSpawnSlotIndex::NONE;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ControlPointValue = 0;

	int32 SpawnCount = 0;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	EMinionSpawnSlotIndex GetSlotIndex() const {return SlotIndex;}

	UFUNCTION()
	void SetMinionTarget(int32 _Value);

	UFUNCTION()
	void StartMinionSpawn();
};
