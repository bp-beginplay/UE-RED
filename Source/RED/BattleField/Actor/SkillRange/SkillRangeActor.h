// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillRangeActor.generated.h"

UCLASS()
class RED_API ASkillRangeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASkillRangeActor();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UCapsuleComponent* Capsule;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* StaticMesh;

	UPROPERTY()
	class USkillComponent* SkillComponent = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetSkillComponent(USkillComponent* _SkillComponent) { SkillComponent = _SkillComponent; }

};
