// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCamp.h"

#include "BaseCampStructure.h"
#include "DrawerComponent/FOW_Drawer_FOV.h"

ABaseCamp::ABaseCamp(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bReplicates = true;

	FOWDrawer_FOV = CreateDefaultSubobject<UFOW_Drawer_FOV>(TEXT("FOW_Drawer_FOV_Comp"));
	FOWDrawer_FOV->SetDrawerRadius(2500);
}

void ABaseCamp::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		FVector SpawnLocation = GetActorLocation() - GetActorForwardVector() * 400.f;
		SpawnLocation.Z += 200.f;
		ABaseCampStructure* BaseCampStructure = GetWorld()->SpawnActor<ABaseCampStructure>(
			BaseCampStructureClass,
			SpawnLocation,
			this->GetActorRotation()
		);
		BaseCampStructure->BaseCamp = this;
	}
}