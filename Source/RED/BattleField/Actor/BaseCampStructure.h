// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RED/Info/EnumInfo.h"
#include "RED/Interface/ActorInterface.h"
#include "BaseCampStructure.generated.h"

UCLASS()
class RED_API ABaseCampStructure : public AActor, public IActorInterface
{
	GENERATED_BODY()
	
public:	
	ABaseCampStructure();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	virtual void BeginPlay() override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	UPROPERTY(EditAnywhere)
	ETeam Team = ETeam::ET_NoTeam;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* BaseCampMesh;

	UPROPERTY(EditAnywhere)
	class UCapsuleComponent* CapsuleCollision;

	UPROPERTY()
	class ABaseCamp* BaseCamp;

	UPROPERTY(EditAnywhere)
	float Health = 100.f;

	virtual int32 GetTeam_Implementation() const override
	{
		return static_cast<int32>(Team);
	}

	virtual ETeam GetETeam_Implementation() const override
	{
		return Team;
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MiniMapIcon;
};
