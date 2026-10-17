// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(BoxCollision);
	BoxCollision->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	ProjectileMovement->bSweepCollision = false;
}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AProjectile::BeginOverlap);

	SpawnSound();
}

void AProjectile::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

void AProjectile::SpawnSound()
{
	if (SkillActiveSoundCue)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			SkillActiveSoundCue,
			GetActorLocation()
		);
	}
}

void AProjectile::DestroyEffect()
{
	if (DestroySoundCue)
	{
		UGameplayStatics::SpawnSoundAtLocation(
			this,
			DestroySoundCue,
			GetActorLocation()
		);
	}
	if (DestroyNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyNiagara,
			GetActorLocation(),
			GetActorRotation(),
			FVector(1.0f), // Scale
			true,          // AutoDestroy
			true,          // AutoActivate
			ENCPoolMethod::None,
			true           // PreCullCheck
		);
	}
}