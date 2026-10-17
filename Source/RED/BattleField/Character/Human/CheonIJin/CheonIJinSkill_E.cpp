// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJinSkill_E.h"

#include "Engine/SkeletalMeshSocket.h"

#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/Actor/Projectile/Projectile.h"

void UCheonIJinSkill_E::SkillSetTarget()
{
	Super::SkillSetTarget();
}

void UCheonIJinSkill_E::SkillStart(FVector _TargetLocation)
{
	if (SkillState != ESkillState::SetTarget)
		return;

	Super::SkillStart(_TargetLocation);
}

void UCheonIJinSkill_E::SkillActive()
{
	PlayAnimMontage(SkillActiveMontage);
}

void UCheonIJinSkill_E::PlayAnimMontage(UAnimMontage* _Montage)
{
	FVector TargetRotation = TargetLocation - PlayerCharacter->GetActorLocation();
	TargetRotation.Z = 0;
	PlayerCharacter->SetActorRotation(TargetRotation.Rotation());
	Super::PlayAnimMontage(_Montage);
}

void UCheonIJinSkill_E::Server_Fire_Implementation()
{
	const USkeletalMeshSocket* MuzzleFlashSocket = PlayerCharacter->GetMesh()->GetSocketByName("Weapon_R");
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(PlayerCharacter->GetMesh());
		FVector ToTarget = TargetLocation - SocketTransform.GetLocation();
		ToTarget.Z = 50.f;
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = PlayerCharacter;
		AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		SpawnedProjectile->StartLocation = PlayerCharacter->GetActorLocation();
	}
	Server_ChangeSkillState(ESkillState::Cooldown);
}

void UCheonIJinSkill_E::SkillCooldown()
{
	Client_SetSkillIconOpacity(false);
	Client_UpdateCoolTimeTextBlock(CoolTime);
	Client_ChangeSkillState(ESkillState::CanActive, CoolTime);
}

void UCheonIJinSkill_E::SkillCanActive()
{
	Client_SetSkillIconOpacity(true);
}
