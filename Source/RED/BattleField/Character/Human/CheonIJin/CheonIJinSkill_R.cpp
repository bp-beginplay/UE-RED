// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJinSkill_R.h"

#include "Engine/SkeletalMeshSocket.h"

#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/Actor/Projectile/Projectile.h"

void UCheonIJinSkill_R::SkillSetTarget()
{
	Super::SkillSetTarget();
}

void UCheonIJinSkill_R::SkillStart(FVector _TargetLocation)
{
	if (SkillState != ESkillState::SetTarget)
		return;

	Super::SkillStart(_TargetLocation);
}

void UCheonIJinSkill_R::SkillActive()
{
	PlayAnimMontage(SkillActiveMontage);
}

void UCheonIJinSkill_R::PlayAnimMontage(UAnimMontage* _Montage)
{
	FVector TargetRotation = TargetLocation - PlayerCharacter->GetActorLocation();
	TargetRotation.Z = 0;
	PlayerCharacter->SetActorRotation(TargetRotation.Rotation());
	Super::PlayAnimMontage(_Montage);
}

void UCheonIJinSkill_R::Server_Fire_Implementation()
{
	UWorld* World = GetWorld();
	if (World && PlayerCharacter)
	{
		FVector PlayerLocation = PlayerCharacter->GetActorLocation();
		FVector ToTarget = TargetLocation - PlayerLocation;
		ToTarget.Z = 0.f;
		FRotator TargetRotation = ToTarget.Rotation();
		PlayerLocation.Z = 1000.f;
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = PlayerCharacter;
		AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, PlayerLocation, TargetRotation, SpawnParams);
		SpawnedProjectile->StartLocation = PlayerCharacter->GetActorLocation();
	}
	Server_ChangeSkillState(ESkillState::Cooldown);
}

void UCheonIJinSkill_R::SkillCooldown()
{
	Client_SetSkillIconOpacity(false);
	Client_UpdateCoolTimeTextBlock(CoolTime);
	Client_ChangeSkillState(ESkillState::CanActive, CoolTime);
}

void UCheonIJinSkill_R::SkillCanActive()
{
	Client_SetSkillIconOpacity(true);
}
