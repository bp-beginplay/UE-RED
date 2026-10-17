// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Actor/Projectile/Projectile.h"
#include "CheonIJin_W_Projectile.generated.h"

UCLASS()
class RED_API ACheonIJin_W_Projectile : public AProjectile
{
	GENERATED_BODY()
	
public:
	ACheonIJin_W_Projectile();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;

	UFUNCTION()
	virtual void ExplosionOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
