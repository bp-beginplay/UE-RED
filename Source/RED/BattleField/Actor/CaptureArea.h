// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RED/Info/EnumInfo.h"
#include "RED/Interface/ActorInterface.h"
#include "CaptureArea.generated.h"

UCLASS()
class RED_API ACaptureArea : public AActor, public IActorInterface
{
	GENERATED_BODY()
	
public:	
	ACaptureArea();
	virtual void BeginPlay() override;

	void SetSphereCollision();
	
	UFUNCTION()
	virtual void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void EndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void DecideTeam();
	void SetCircleArea(ETeam _Team);
	void SetCircleScale(class UNiagaraComponent* _CircleComponent, ETeam _Team);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetControlPointTeam(ETeam _Team);

	UPROPERTY()
	TArray<AActor*> ActorArray;

	UPROPERTY(EditAnywhere)
	class USphereComponent* SphereCollision;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseCamp> RedBaseCampClass;
	UPROPERTY(EditAnywhere)
	TSubclassOf<ABaseCamp> BlueBaseCampClass;
	UPROPERTY(EditAnywhere)
	float CaptureTime = 5.f;
	FTimerHandle CircleScaleTHandle;
	float CountDown = 0.f;
	bool bBaseCamp = false;

	UPROPERTY(EditAnywhere)
	UNiagaraComponent* RedCircleComponent;
	UPROPERTY(EditAnywhere)
	UNiagaraComponent* BlueCircleComponent;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UChildActorComponent> ControlPoint;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 ControlPointValue;

public:
	int32 GetControlPointValue() const { return ControlPointValue; }
};
