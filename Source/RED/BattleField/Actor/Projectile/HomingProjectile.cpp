// Fill out your copyright notice in the Description page of Project Settings.


#include "HomingProjectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "RED/Interface/ActorInterface.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/Actor/BaseCampStructure.h"
#include "RED/BattleField/Character/Devil/Yaco/Yaco.h"
#include "RED/BattleField/Character/Minion/BaseMinionCharacter.h"
#include "RED/BattleField/Character/Human/CheonIJin/CheonIJin.h"
#include "RED/BattleField/Character/Human/CheonIJin/CheonIJin_Treasure.h"

AHomingProjectile::AHomingProjectile()
{
	ProjectileMovement->bIsHomingProjectile = true;
	ProjectileMovement->HomingAccelerationMagnitude = 200000.f;
	ProjectileMovement->bSweepCollision = true;
	SetReplicateMovement(true);
}

void AHomingProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(TargetActor) && !IActorInterface::Execute_GetIsDied(TargetActor))
	{
		FVector TargerVector = TargetActor->GetActorLocation() - GetActorLocation();
		SetActorRotation(TargerVector.Rotation());
	}
	else
	{
		Destroy();
	}
}

void AHomingProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor == GetOwner())
	{
		return;
	}

	if (OtherComp->GetOwner() == GetOwner())
	{
		return;
	}

	APlayerCharacter* InstigatorCharacter = Cast<APlayerCharacter>(GetInstigator());
	ABattleFieldPlayerState* InstigatorPlayerState = Cast<ABattleFieldPlayerState>(InstigatorCharacter->GetPlayerState());

	float totalDamage = 0.f;

	// 미리 계산하고 apply를 통해 데미지를 주는게 아니라 데미지만 주고 take쪽에서 계산하는게 훨씬 자연스러움
	// applydamage 하나면 끝날걸 3개의 경우로 나눠야하는 문제 생김
	APlayerCharacter* TargetCharacter = Cast<APlayerCharacter>(OtherActor);
	if (TargetCharacter)
	{
		ABattleFieldPlayerState* TargetPlayerState = Cast<ABattleFieldPlayerState>(TargetCharacter->GetPlayerState());
		float CaculateDamage = InstigatorPlayerState->TotalStats.Attack - TargetPlayerState->TotalStats.Defense;
		totalDamage = UGameplayStatics::ApplyDamage(OtherActor, CaculateDamage, GetInstigatorController(), this, UDamageType::StaticClass());

		ABattleFieldPlayerState* DamagePlayerState = Cast<ABattleFieldPlayerState>(TargetCharacter->GetPlayerState());
		if (DamagePlayerState->Health <= 0.f)
		{
			ACheonIJin* CheonIJin = Cast<ACheonIJin>(InstigatorCharacter);
			if (CheonIJin)
			{
				ACheonIJin_Treasure* BaseCamp = GetWorld()->SpawnActor<ACheonIJin_Treasure>(
					CheonIJinTresureClass,
					TargetCharacter->GetActorLocation(),
					TargetCharacter->GetActorRotation());
				BaseCamp->SetOwner(CheonIJin);
			}
		}
	}

	if (ABaseMinionCharacter* Minion = Cast<ABaseMinionCharacter>(OtherActor))
	{
		totalDamage = UGameplayStatics::ApplyDamage(OtherActor, InstigatorPlayerState->TotalStats.Attack, GetInstigatorController(), this, UDamageType::StaticClass());
	}

	ABaseCampStructure* BaseCampStructure = Cast<ABaseCampStructure>(OtherActor);
	if (BaseCampStructure)
	{
		float CaculateDamage = InstigatorPlayerState->TotalStats.Attack;
		totalDamage = UGameplayStatics::ApplyDamage(OtherActor, CaculateDamage, GetInstigatorController(), this, UDamageType::StaticClass());
	}

	DestroyEffect();

	APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner());
	if (player)
	{
		player->LifeSteal(totalDamage);
	}

	// applydamage이후에 바로 부수면 가끔 터질수도 있음
	// 유의
	Destroy();
}

void AHomingProjectile::SetHomingTarget(AActor* _TargetActor)
{
	if (!_TargetActor) return;
	TargetActor = _TargetActor;
	ProjectileMovement->HomingTargetComponent = _TargetActor->GetRootComponent();
}