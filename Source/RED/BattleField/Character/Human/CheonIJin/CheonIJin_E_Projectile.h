// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Actor/Projectile/Projectile.h"
#include "CheonIJin_E_Projectile.generated.h"

UCLASS()
class RED_API ACheonIJin_E_Projectile : public AProjectile
{
	GENERATED_BODY()
	
public:
	ACheonIJin_E_Projectile();
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;

	virtual void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;
};
