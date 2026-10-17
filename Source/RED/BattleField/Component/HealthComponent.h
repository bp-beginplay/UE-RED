// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnHPChanged, float /*NewHP*/, float /*Shield*/, float /*MaxHP*/);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RED_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_BarChanged)
	float MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_BarChanged)
	float HP;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRep_BarChanged)
	float Shield = 0.f;

	UFUNCTION()
	void OnRep_BarChanged();

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION(NetMulticast, Reliable)
	void Mul_NotifyHPChanged(class UWidgetComponent* _WidgetComp,float _HP, float _ShieldAmount,  float _MaxHP);
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void HandleDamage(float DamageAmount);

	FOnHPChanged OnHPChanged;

	void SetHP(float _HP) { HP = _HP; }
	float GetHP() const { return HP; }

	void SetMaxHP(float _MaxHP) { MaxHP = _MaxHP; }
	float GetMaxHP() const { return MaxHP; }
};
