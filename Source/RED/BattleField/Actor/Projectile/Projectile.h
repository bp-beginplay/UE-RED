// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

UCLASS()
class RED_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditAnywhere)
	class UBoxComponent* BoxCollision;

	UPROPERTY(EditAnywhere)
	TObjectPtr<class UProjectileMovementComponent> ProjectileMovement;

	FVector TargetLocation;
	FVector StartLocation;

	void SpawnSound();

	UPROPERTY(EditAnywhere)
	class USoundCue* SkillActiveSoundCue;
	UPROPERTY(EditAnywhere)
	USoundCue* DestroySoundCue;
	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* DestroyNiagara;

	void DestroyEffect();
};
