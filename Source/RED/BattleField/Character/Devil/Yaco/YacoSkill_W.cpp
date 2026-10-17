// Fill out your copyright notice in the Description page of Project Settings.


#include "YacoSkill_W.h"

#include "Yaco.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/Actor/SkillRange/SkillRangeActor.h"
#include "RED/BattleField/Character/BaseCharacter.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/DamageType/SkillDamageType.h"

UYacoSkill_W::UYacoSkill_W()
{
	PrimaryComponentTick.bCanEverTick = true;
	CoolTime = 5;
}

void UYacoSkill_W::Server_Fire_Implementation()
{
	Super::Server_Fire_Implementation();

	Server_SpawnSkill(TargetLocation);

	Server_ChangeSkillState(ESkillState::Cooldown);

	Client_SetSkillIconOpacity(false);
	Client_UpdateCoolTimeTextBlock(CoolTime);
	Client_ChangeSkillState(ESkillState::CanActive, CoolTime);
}

void UYacoSkill_W::SkillStart()
{
	Super::SkillStart();
}

void UYacoSkill_W::SkillStart(FVector _TargetLocation)
{
	if (SkillState != ESkillState::SetTarget)
	{
		return;
	}

	Super::SkillStart(_TargetLocation);
}

void UYacoSkill_W::SkillActive()
{
	Super::SkillActive();

	FVector TargetDistance = TargetLocation - PlayerCharacter->GetActorLocation();
	TargetDistance.Z = 0;
	if (ArrangeRadius < TargetDistance.Length())
	{
		bAutoRun = true;
	}
	else
	{
		bAutoRun = false;
		PlayAnimMontage(SkillActiveMontage);
	}
}

void UYacoSkill_W::SkillCooldown()
{
	Super::SkillCooldown();
}

void UYacoSkill_W::SkillCanActive()
{
	Super::SkillCanActive();

	Client_SetSkillIconOpacity(true);
}

void UYacoSkill_W::AutoRun()
{
	Super::AutoRun();

	if (bAutoRun)
	{
		FVector TargetDistance = TargetLocation - PlayerCharacter->GetActorLocation();
		TargetDistance.Z = 0;
		const FVector WorldDirection = TargetDistance.GetSafeNormal();
		PlayerCharacter->AddMovementInput(WorldDirection);
		if (ArrangeRadius >= TargetDistance.Length())
		{
			bAutoRun = false;
			PlayAnimMontage(SkillActiveMontage);
		}
	}
}

void UYacoSkill_W::TickComponent(float DeltaTime, enum ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AutoRun();
}

void UYacoSkill_W::ApplyDamageNDestroy(ASkillRangeActor* SkillRangeActor)
{
	if (!IsValid(GetOwner()))
	{
		return;
	}

	TArray<AActor*> OverlappingActors;
	SkillRangeActor->Capsule->UpdateOverlaps();
	SkillRangeActor->Capsule->GetOverlappingActors(OverlappingActors);

	for (auto& Actor : OverlappingActors)
	{
		if (!IsValid(Actor))
		{
			continue;
		}

		if (IActorInterface::Execute_GetIsDied(Actor))
		{
			continue;
		}

		if (IActorInterface::Execute_GetETeam(Actor) == IActorInterface::Execute_GetETeam(GetOwner()))
		{
			continue;
		}

		SkillRangeActor->SetSkillComponent(this);

		float damage = UGameplayStatics::ApplyDamage(
				Actor,
				10.f,
				GetOwner()->GetInstigatorController(),
			SkillRangeActor,
				UDamageType::StaticClass()
		);

		if (damage > 0.f)
		{
			Cast<APlayerCharacter>(GetOwner())->LifeSteal(damage);
		}
	}

	Server_RequestPlayEffect(SkillRangeActor);
}

void UYacoSkill_W::Server_RequestPlayEffect_Implementation(ASkillRangeActor* SkillRangeActor)
{
	Mul_PlayEffect(SkillRangeActor);
}

void UYacoSkill_W::Mul_PlayEffect_Implementation(ASkillRangeActor* SkillRangeActor)
{
	if (!IsValid(SkillRangeActor))
	{
		return;
	}

	UGameplayStatics::SpawnEmitterAtLocation(
		GetWorld(),
		ParticleSystem,     // UParticleSystem*
		SkillRangeActor->GetActorLocation(),
		SkillRangeActor->GetActorRotation(),
		FVector(1.0f)       // Scale
	);

	SkillRangeActor->Destroy();
}

void UYacoSkill_W::Server_SpawnSkill_Implementation(FVector CenterLocation)
{
	ABattleFieldPlayerController* BPC = Cast<ABattleFieldPlayerController>(GetOwner()->GetInstigatorController());

	if (BPC)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = GetOwner()->GetInstigator();
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		FVector Location{ CenterLocation.X, CenterLocation.Y, 20 };

		ASkillRangeActor* WSkill = GetWorld()->SpawnActor<ASkillRangeActor>(
			ASkillRangeActor::StaticClass(),
			Location,
			FRotator::ZeroRotator,
			SpawnParams
		);

		FTimerHandle DamageTimer;
		GetWorld()->GetTimerManager().SetTimer(
			DamageTimer,
			FTimerDelegate::CreateLambda([this, WSkill]()
				{
					ApplyDamageNDestroy(WSkill);
				}),
			1.f,
			false
		);
	}
}
