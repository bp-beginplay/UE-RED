// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJin_R_Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "RED/Interface/ActorInterface.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/Actor/Projectile/HomingProjectile.h"
#include "RED/BattleField/Character/BaseCharacter.h"

ACheonIJin_R_Projectile::ACheonIJin_R_Projectile()
{
	//ShipCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("ShipCollision"));

}

void ACheonIJin_R_Projectile::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ACheonIJin_R_Projectile::EndOverlap);
}

void ACheonIJin_R_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Distance = StartLocation - GetActorLocation();
	Distance.Z = 0;
	if (Distance.Length() >= 3000.f)
	{
		Destroy();
	}
}

void ACheonIJin_R_Projectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetInstigator());
	if (Player->IsEnemy(OtherActor))
	{
		TargetActorArray.Add(OtherActor);

		if (!IsFire)
		{
			IsFire = true;
			TargetFire();
		}
	}
}

void ACheonIJin_R_Projectile::EndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TargetActorArray.Remove(OtherActor);
	if (TargetActorArray.IsEmpty())
	{
		IsFire = false;
	}
}

void ACheonIJin_R_Projectile::TargetFire()
{
	for (auto& iter : TargetActorArray)
	{
		UWorld* World = GetWorld();
		if (World && iter)
		{
			FTransform SocketTransform = GetActorTransform();
			FVector ToTarget = iter->GetActorLocation() - SocketTransform.GetLocation();
			FRotator TargetRotation = ToTarget.Rotation();

			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = GetInstigator();
			AHomingProjectile* SpawnedHomingProjectile = 
				World->SpawnActor<AHomingProjectile>(ProjectileClass, SocketTransform.GetLocation(), TargetRotation, SpawnParams);
			if (SpawnedHomingProjectile)
			{
				SpawnedHomingProjectile->SetHomingTarget(iter);
			}
		}
	}
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&ACheonIJin_R_Projectile::TargetFire,
		1.f
	);
}