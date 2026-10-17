// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "RED/Info/EnumInfo.h"
#include "RED/Interface/ActorInterface.h"
#include "BaseCamp.generated.h"

UCLASS()
class RED_API ABaseCamp : public APlayerStart, public IActorInterface
{
	GENERATED_BODY()

public:
	ABaseCamp(const FObjectInitializer& ObjectInitializer);

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = "true"))
	ETeam Team = ETeam::ET_NoTeam;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ABaseCampStructure> BaseCampStructureClass;

	virtual ETeam GetETeam_Implementation() const override
	{
		return Team;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FOW")
	class UFOW_Drawer_FOV* FOWDrawer_FOV;
};
