// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJin_E_Projectile.h"

#include "Components/SphereComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/SkeletalMeshSocket.h"

#include "RED/Info/StructInfo.h"
#include "RED/Info/RedGamePlayTags.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/BattleFieldPlayerState.h"

ACheonIJin_E_Projectile::ACheonIJin_E_Projectile()
{
	//phereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	//SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//SetRootComponent(SphereCollision);
}

void ACheonIJin_E_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Distance = StartLocation - GetActorLocation();
	Distance.Z = 0.f;

	if (HasAuthority())
	{
		if (Distance.Length() >= 1000.f)
		{
			Destroy();
		}
	}

}

void ACheonIJin_E_Projectile::BeginPlay()
{
	Super::BeginPlay();

	//SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheonIJin_E_Projectile::StunOverlap);
}

void ACheonIJin_E_Projectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* TargetCharacter = Cast<APlayerCharacter>(OtherActor);
	APlayerCharacter* Player = Cast<APlayerCharacter>(GetInstigator());
	if (Player->IsEnemy(TargetCharacter))
	{
		FBuffData StunDeBuff;
		StunDeBuff.Duration = 1.25f;
		StunDeBuff.BuffTag = FRedGameplayTags::Buff_CheonIJin_E_Stun;
		StunDeBuff.BuffType = EBuffType::REFRESHONSTACK;
		StunDeBuff.MaxCount = 1;

		ABattleFieldPlayerState* TargetPS = Cast<ABattleFieldPlayerState>(TargetCharacter->GetPlayerState());
		TargetPS->AddDeBuffs(StunDeBuff);

		if (ImpactParticles)
		{
			const USkeletalMeshSocket* MuzzleFlashSocket = TargetCharacter->GetMesh()->GetSocketByName("FX_Head");
			FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(TargetCharacter->GetMesh());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, SocketTransform);
			
			//StunParticle->bAutoDestroy = true;
		}

		Destroy();
	}
}
