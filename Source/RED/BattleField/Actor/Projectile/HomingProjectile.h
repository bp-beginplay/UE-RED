// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"

#include "HomingProjectile.generated.h"

UCLASS()
class RED_API AHomingProjectile : public AProjectile
{
	GENERATED_BODY()
	
public:	
	AHomingProjectile();
	virtual void Tick(float DeltaTime) override;

	virtual void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;


	void SetHomingTarget(AActor* TargetActor);

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACheonIJin_Treasure> CheonIJinTresureClass;
};
