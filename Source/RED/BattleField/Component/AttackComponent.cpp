// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackComponent.h"

#include "Engine/SkeletalMeshSocket.h"

#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/Actor/Projectile/HomingProjectile.h"

UAttackComponent::UAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAttackComponent::SkillSetTarget()
{
	Super::SkillSetTarget();
}

void UAttackComponent::SkillStart(FVector _TargetLocation)
{
	if (SkillState != ESkillState::SetTarget)
		return;

	Server_SetTargetLocation(_TargetLocation);
	Client_ChangeSkillState(ESkillState::Active, ActiveTime);
}

void UAttackComponent::SkillActive()
{
	if (!PlayerCharacter->TargetActorArray.IsEmpty())
	{
		bAutoRun = false;
		NearistTargetActor();
		PlayAnimMontage(SkillActiveMontage);
	}
	else
	{
		bAutoRun = true;
	}

	Client_ChangeSkillState(ESkillState::CanActive, ActiveTime);
}

void UAttackComponent::NearistTargetActor()
{
	TArray<AActor*> TargetActorArray = PlayerCharacter->TargetActorArray;
	FVector TargetVector;
	float Distance = 0;
	for (auto& iter : TargetActorArray)
	{
		TargetVector = iter->GetActorLocation() - TargetLocation;
		if (Distance == 0)
		{
			Distance = TargetVector.Length();
			TargetActor = iter;
		}

		if (Distance > TargetVector.Length())
		{
			TargetActor = iter;
		}
	}
}

void UAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AutoRun();
}

void UAttackComponent::AutoRun()
{
	if (bAutoRun)
	{
		if (TargetActor)
		{
			if (IActorInterface::Execute_GetVisibility(TargetActor) != 0)
			{
				TargetLocation = TargetActor->GetActorLocation();
			}
			else
			{
				bAutoRun = false;
				return;
			}
		}
		FVector TargetDistance = TargetLocation - PlayerCharacter->GetActorLocation();
		TargetDistance.Z = 0;
		const FVector WorldDirection = TargetDistance.GetSafeNormal();
		PlayerCharacter->AddMovementInput(WorldDirection);
		if (20.f >= TargetDistance.Length())
		{
			bAutoRun = false;
		}
		if (!PlayerCharacter->TargetActorArray.IsEmpty())
		{
			Server_ChangeSkillState(ESkillState::Active);
		}
	}
}

void UAttackComponent::EndAnimMontage()
{
	if (IsValid(TargetActor) && !IActorInterface::Execute_GetIsDied(TargetActor))
	{
		FVector TargetDistance = TargetActor->GetActorLocation() - PlayerCharacter->GetActorLocation();
		TargetDistance.Z = 0;
		if (ArrangeRadius >= TargetDistance.Length())
		{
			PlayAnimMontage(SkillActiveMontage);
		}
		else
		{
			bAutoRun = true;
		}
	}
	else
	{
		bAutoRun = false;
		Server_ChangeSkillState(ESkillState::CanActive);
	}
}

void UAttackComponent::PlayAnimMontage(UAnimMontage* _Montage)
{
	FVector TargetRotation = TargetActor->GetActorLocation() - PlayerCharacter->GetActorLocation();
	TargetRotation.Z = 0.f;
	PlayerCharacter->SetActorRotation(TargetRotation.Rotation());
	Super::PlayAnimMontage(_Montage);
}

void UAttackComponent::Server_Fire_Implementation()
{
	const USkeletalMeshSocket* MuzzleFlashSocket = PlayerCharacter->GetMesh()->GetSocketByName("Weapon_R");
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World && TargetActor)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(PlayerCharacter->GetMesh());
		FVector ToTarget = TargetActor->GetActorLocation() - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = PlayerCharacter;
		AHomingProjectile* SpawnedHomingProjectile = World->SpawnActor<AHomingProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		if (SpawnedHomingProjectile)
		{
			SpawnedHomingProjectile->SetHomingTarget(TargetActor);
		}
	}

	Server_ChangeSkillState(ESkillState::CanActive);
}
