// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RED/BattleField/Actor/Projectile/Projectile.h"
#include "CheonIJin_R_Projectile.generated.h"

/**
 * 
 */
UCLASS()
class RED_API ACheonIJin_R_Projectile : public AProjectile
{
	GENERATED_BODY()
	
public:
	ACheonIJin_R_Projectile();

	void BeginPlay() override;

	void Tick(float DeltaTime) override;

	virtual void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
	
	UFUNCTION()
	void EndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	TArray<AActor*> TargetActorArray;

	void TargetFire();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;

	bool IsFire = false;
};
