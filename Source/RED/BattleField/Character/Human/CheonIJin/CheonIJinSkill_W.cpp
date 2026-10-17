// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJinSkill_W.h"

#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/Actor/Projectile/Projectile.h"
#include "RED/BattleField/UserWidget/SkillIcon.h"

UCheonIJinSkill_W::UCheonIJinSkill_W()
{
	PrimaryComponentTick.bCanEverTick = true;
	SkillCount = InitSkillCount;
}

void UCheonIJinSkill_W::BeginPlay()
{
	Super::BeginPlay();

	if (SkillIcon)
		SkillIcon->SkillCountTextBlock->SetText(FText::AsNumber(InitSkillCount));
}

void UCheonIJinSkill_W::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AutoRun();
	UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(SkillState));
}

void UCheonIJinSkill_W::AutoRun()
{
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

void UCheonIJinSkill_W::SkillSetTarget()
{
	if (SkillCount == 0)
		return;

	Super::SkillSetTarget();
}

void UCheonIJinSkill_W::SkillStart(FVector _TargetLocation)
{
	if (SkillState != ESkillState::SetTarget)
		return;

	Super::SkillStart(_TargetLocation);
}

void UCheonIJinSkill_W::SkillActive()
{
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

void UCheonIJinSkill_W::SkillCanActive()
{
	Server_UpdateSkillCountTextBlock();

	Client_SetSkillIconOpacity(SkillCount != 0);

	if (!GetOwner()->HasAuthority())
		return;
	if (SkillCount != InitSkillCount)
	{
		Client_SkillCountTime();
	}
}

void UCheonIJinSkill_W::PlayAnimMontage(UAnimMontage* _Montage)
{
	FVector TargetRotation = TargetLocation - PlayerCharacter->GetActorLocation();
	TargetRotation.Z = 0.f;
	PlayerCharacter->SetActorRotation(TargetRotation.Rotation());
	Super::PlayAnimMontage(_Montage);
}

void UCheonIJinSkill_W::Server_Fire_Implementation()
{
	const USkeletalMeshSocket* MuzzleFlashSocket = PlayerCharacter->GetMesh()->GetSocketByName("Weapon_R");
	UWorld* World = GetWorld();
	if (MuzzleFlashSocket && World)
	{
		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(PlayerCharacter->GetMesh());
		FVector ToTarget = TargetLocation - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		SpawnParams.Instigator = PlayerCharacter;
		AProjectile* SpawnedProjectile = World->SpawnActor<AProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
		SpawnedProjectile->TargetLocation = TargetLocation;
	}
	--SkillCount;

	Server_ChangeSkillState(ESkillState::CanActive);
}