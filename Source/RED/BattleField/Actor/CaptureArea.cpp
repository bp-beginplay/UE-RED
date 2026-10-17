// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureArea.h"

#include "Components/SphereComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "ControlPoint/ControlPointBaseActor.h"

#include "BaseCamp.h"
#include "BaseCampStructure.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/Interface/ActorInterface.h"
#include "RED/BattleField/BattleFieldGameMode.h"
#include "RED/BattleField/BattleFieldGameState.h"

ACaptureArea::ACaptureArea()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetRootComponent(SphereCollision);

	RedCircleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RedCircle"));
	RedCircleComponent->SetupAttachment(SphereCollision);
	RedCircleComponent->SetVisibility(false);

	BlueCircleComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("BlueCircleArea"));
	BlueCircleComponent->SetupAttachment(SphereCollision);
	BlueCircleComponent->SetVisibility(false);

	ControlPoint = CreateDefaultSubobject<UChildActorComponent>(TEXT("ControlPoint"));
	ControlPoint->SetupAttachment(SphereCollision);
}

void ACaptureArea::BeginPlay()
{
	Super::BeginPlay();
 
	SphereCollision->OnComponentBeginOverlap.AddDynamic(this, &ACaptureArea::BeginOverlap);
	SphereCollision->OnComponentEndOverlap.AddDynamic(this, &ACaptureArea::EndOverlap);

	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(
		TimerHandle,
		this,
		&ACaptureArea::SetSphereCollision,
		3
	);
}

void ACaptureArea::BeginOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor->Implements<UActorInterface>())return;
	if(ABaseCampStructure* BaseCamp = Cast<ABaseCampStructure>(OtherActor))
	{
		bBaseCamp = true;
		switch(IActorInterface::Execute_GetETeam(BaseCamp))
		{
		case ETeam::ET_BlueTeam:
			SetControlPointTeam(ETeam::ET_BlueTeam);
			break;
		case ETeam::ET_RedTeam:
			SetControlPointTeam(ETeam::ET_RedTeam);
			break;
		case ETeam::ET_NoTeam:
			SetControlPointTeam(ETeam::ET_NoTeam);
			break;
		}
	}
	ActorArray.Add(OtherActor);
	DecideTeam();
}

void ACaptureArea::EndOverlap(UPrimitiveComponent* OverlappedComponent, 
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!OtherActor->Implements<UActorInterface>())return;
	if (ABaseCampStructure* BaseCamp = Cast<ABaseCampStructure>(OtherActor))
	{
		bBaseCamp = false;
		SetControlPointTeam(ETeam::ET_NoTeam);
	}
	ActorArray.Remove(OtherActor);
	DecideTeam();
}

void ACaptureArea::DecideTeam()
{
	bool bRedTeam = false;
	bool bBlueTeam = false;
	APlayerCharacter* PlayerCharacter = nullptr;
	for (auto& iter : ActorArray)
	{
		PlayerCharacter = Cast<APlayerCharacter>(iter);
		switch (IActorInterface::Execute_GetETeam(iter))
		{
		case ETeam::ET_RedTeam:
			bRedTeam = true;
			break;
		case ETeam::ET_BlueTeam:
			bBlueTeam = true;
			break;
		case ETeam::ET_NoTeam:
			break;
		default:
			break;
		}
	}

	if (bRedTeam)
	{
		if (!bBlueTeam && PlayerCharacter && !bBaseCamp)
		{
			SetCircleArea(ETeam::ET_RedTeam);
		}
		else
		{
			SetCircleArea(ETeam::ET_NoTeam);
		}
	}
	else
	{
		if (bBlueTeam && PlayerCharacter && !bBaseCamp)
		{
			SetCircleArea(ETeam::ET_BlueTeam);
		}
		else
		{
			SetCircleArea(ETeam::ET_NoTeam);
		}
	}
}

void ACaptureArea::SetCircleArea(ETeam _Team)
{
	switch (_Team)
	{
	case ETeam::ET_RedTeam:
		RedCircleComponent->SetVisibility(true);
		GetWorld()->GetTimerManager().SetTimer(
			CircleScaleTHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					SetCircleScale(RedCircleComponent, ETeam::ET_RedTeam);
				}),
			0.01f,
			true
		);
		break;
	case ETeam::ET_BlueTeam:
		BlueCircleComponent->SetVisibility(true);
		GetWorld()->GetTimerManager().SetTimer(
			CircleScaleTHandle,
			FTimerDelegate::CreateLambda([this]()
				{
					SetCircleScale(BlueCircleComponent, ETeam::ET_BlueTeam);
				}),
			0.01f,
			true
		);
		break;
	case ETeam::ET_NoTeam:
		GetWorld()->GetTimerManager().ClearTimer(CircleScaleTHandle);
		CountDown = 0.f;
		RedCircleComponent->SetVisibility(false);
		BlueCircleComponent->SetVisibility(false);
		RedCircleComponent->SetWorldScale3D(FVector3d(1, 1, 1));
		BlueCircleComponent->SetWorldScale3D(FVector3d(1, 1, 1));
		break;
	default:
		break;
	}
}

void ACaptureArea::SetCircleScale(UNiagaraComponent* _CircleComponent, ETeam _Team)
{
	CountDown += 0.01;
	_CircleComponent->SetWorldScale3D(FVector3d(4 * CountDown, 4 * CountDown, 1));
	if (CountDown >= CaptureTime)
	{
		GetWorld()->GetTimerManager().ClearTimer(CircleScaleTHandle);
		CountDown = 0.f;
		RedCircleComponent->SetVisibility(false);
		BlueCircleComponent->SetVisibility(false);
		RedCircleComponent->SetWorldScale3D(FVector3d(1, 1, 1));
		BlueCircleComponent->SetWorldScale3D(FVector3d(1, 1, 1));

		ABaseCamp* BaseCamp = nullptr;
		FVector3d SpawnLocation = this->GetActorLocation();
		SpawnLocation.Z += 110;

		if (HasAuthority())
		{
			switch (_Team)
			{
			case ETeam::ET_RedTeam:
				BaseCamp = GetWorld()->SpawnActor<ABaseCamp>(
					RedBaseCampClass,
					SpawnLocation,
					this->GetActorRotation()
				);
				break;
			case ETeam::ET_BlueTeam:
				BaseCamp = GetWorld()->SpawnActor<ABaseCamp>(
					BlueBaseCampClass,
					SpawnLocation,
					this->GetActorRotation()
				);
				break;
			default:
				break;
			}
			ABattleFieldGameMode* BattleFieldGameMode = Cast<ABattleFieldGameMode>(GetWorld()->GetAuthGameMode());
			BattleFieldGameMode->UpdateTeamScore(_Team, 1);
		}

	}
}

void ACaptureArea::SetControlPointTeam_Implementation(ETeam _Team)
{
	ABattleFieldGameState* BGS = GetWorld()->GetGameState<ABattleFieldGameState>();
	if (BGS)
	{
		BGS->SetControlPointTeam(ControlPointValue, _Team);
	}
}

void ACaptureArea::SetSphereCollision()
{
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
}
