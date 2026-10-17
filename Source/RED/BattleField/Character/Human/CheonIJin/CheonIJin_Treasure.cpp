// Fill out your copyright notice in the Description page of Project Settings.


#include "CheonIJin_Treasure.h"

#include "Components/SphereComponent.h"

#include "CheonIJin.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "Sound/SoundCue.h"
#include "Kismet/GameplayStatics.h"

ACheonIJin_Treasure::ACheonIJin_Treasure()
{
	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->SetupAttachment(GetRootComponent());
	SphereComponent->InitSphereRadius(100.f);

	SetReplicates(true);
}

void ACheonIJin_Treasure::BeginPlay()
{
	Super::BeginPlay();
	
	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &ACheonIJin_Treasure::BeginOverlap);
}

void ACheonIJin_Treasure::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (GetOwner() == OtherActor)
	{
		ACheonIJin* CheonIJin = Cast<ACheonIJin>(GetOwner());
		ABattleFieldPlayerState* PS = Cast<ABattleFieldPlayerState>(CheonIJin->GetPlayerState());
		PS->Server_AddBaseState(1.f);
		if (DestroySoundCue)
		{
			UGameplayStatics::SpawnSoundAtLocation(
				this,
				DestroySoundCue,
				GetActorLocation()
			);
		}
		Destroy();
	}
}
