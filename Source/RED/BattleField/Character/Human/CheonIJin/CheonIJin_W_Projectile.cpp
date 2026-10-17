// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJin_W_Projectile.h"

#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

#include "RED/FuncLibrary/RedBlueprintFunctionLibrary.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/Character/PlayerCharacter.h"

ACheonIJin_W_Projectile::ACheonIJin_W_Projectile()
{
	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(SphereCollision);
}

void ACheonIJin_W_Projectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FVector Distance = GetActorLocation() - TargetLocation;
	Distance.Z = 0.f;
	if (Distance.Length() <= 50.f)
	{
		SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		DestroyEffect();
		Destroy();
	}
}

void ACheonIJin_W_Projectile::BeginPlay()
{
	Super::BeginPlay();

	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACheonIJin_W_Projectile::ExplosionOverlap);
}

void ACheonIJin_W_Projectile::ExplosionOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* InstigatorCharacter = Cast<APlayerCharacter>(GetInstigator());
	if (!InstigatorCharacter->IsEnemy(OtherActor))
		return;

	ABattleFieldPlayerState* InstigatorPlayerState = Cast<ABattleFieldPlayerState>(InstigatorCharacter->GetPlayerState());

	APlayerCharacter* TargetCharacter = Cast<APlayerCharacter>(OtherActor);
	
	float Defense = 0.f;
	if (TargetCharacter)
	{
		ABattleFieldPlayerState* TargetPlayerState = Cast<ABattleFieldPlayerState>(TargetCharacter->GetPlayerState());
		Defense = TargetPlayerState->TotalStats.Defense;
	}

	float CaculateDamage = URedBlueprintFunctionLibrary::CalculateDamage(InstigatorPlayerState->TotalStats.Attack, Defense);
	UGameplayStatics::ApplyDamage(OtherActor, CaculateDamage, GetInstigatorController(), this, UDamageType::StaticClass());
}
