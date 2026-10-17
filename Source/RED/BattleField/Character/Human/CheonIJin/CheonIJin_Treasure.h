// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheonIJin_Treasure.generated.h"

UCLASS()
class RED_API ACheonIJin_Treasure : public AActor
{
	GENERATED_BODY()
	
public:	
	ACheonIJin_Treasure();
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereComponent;

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
	class USoundCue* DestroySoundCue;
};
