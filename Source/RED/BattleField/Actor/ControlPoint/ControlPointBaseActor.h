// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "RED/Interface/ActorInterface.h"
#include "RED/Info/EnumInfo.h"

#include "ControlPointBaseActor.generated.h"

UCLASS()
class RED_API AControlPointBaseActor : public AActor, public IActorInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AControlPointBaseActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ControlPointValue;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class AMinionSpawnSlotBaseActor*> OwnedMinionSpawnSlotList;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* ControlPointMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team")
	ETeam Team = ETeam::ET_NoTeam;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(BlueprintCallable)
	void SetControlPointValue(int32 _Value) { ControlPointValue = _Value; }

	UFUNCTION(BlueprintCallable)
	int32 GetControlPointValue() const
	{
		return ControlPointValue;
	}

	UFUNCTION()
	void SetMinionMovePoint(ETeam _Team, EMinionSpawnSlotIndex _index, int32 _Value);

	UFUNCTION(Server, Reliable)
	void Server_CheckOverlaps();

	virtual int32 GetTeam_Implementation() const override
	{
		return static_cast<int32>(Team);
	}

	virtual ETeam GetETeam_Implementation() const override
	{
		return Team;
	}
};
