// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"

#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"
#include "RED/BattleField/UserWidget/HPBarWidget.h"

// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
	// ...
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

void UHealthComponent::OnRep_BarChanged()
{
	if (OnHPChanged.IsBound())
	{
		OnHPChanged.Broadcast(HP, Shield, MaxHP);
	}
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, HP);
}

void UHealthComponent::Mul_NotifyHPChanged_Implementation(UWidgetComponent* _WidgetComp, float _HP, float _ShieldAmount, float _MaxHP)
{
	UUserWidget* RawWidget = _WidgetComp->GetUserWidgetObject();

	UHPBarWidget* HPWidget = Cast<UHPBarWidget>(RawWidget);

	if (HPWidget)
	{
		HPWidget->UpdateBar(_HP, _ShieldAmount, _MaxHP);
	}
}

// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UHealthComponent::HandleDamage(float DamageAmount)
{
    if (DamageAmount <= 0.f) return;

    HP = FMath::Clamp(HP - DamageAmount, 0.f, MaxHP);

    if (OnHPChanged.IsBound())
    {

        OnHPChanged.Broadcast(HP, Shield, MaxHP);
    }

    /*if (HP <= 0.f)
    {
		OnHPChanged.Broadcast(HP, MaxHP);
    }*/
}