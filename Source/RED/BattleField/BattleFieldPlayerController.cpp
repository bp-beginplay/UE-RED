// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleFieldPlayerController.h"

#include "BattleFieldGameMode.h"
#include "BattleFieldGameState.h"
#include "Components/SphereComponent.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "Components/Overlay.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Network/Replication/FOW_FogStateReplication_Client.h"
#include "Network/Replication/FOW_FogStateReplication_Server.h"
#include "RED/BattleField/BattleFieldHUD.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/Character/BaseCharacter.h"
#include "RED/Interface/ActorInterface.h"
#include "RED/RED.h"
#include "RED/Info/RedGamePlayTags.h"
#include "RED/Interface/SelectControlZoneUI.h"
#include "Tools/FunctionLibrairy/FOWFunctionLibrary.h"
#include "UserWidget/PlayerOverlay.h"

ABattleFieldPlayerController::ABattleFieldPlayerController()
{
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
}

void ABattleFieldPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	if (PlayerCharacter && !IActorInterface::Execute_GetIsDied(PlayerCharacter))
	{
		CursorTrace();

		AutoRun();
	}

	if (!IsFixCamera)
	{
		if (Map_LMB_Clicked)
		{
			PlayerCharacter->SpringArm->SetRelativeLocation(TargetLocation);
		}
		else if (Space_Pressed)
		{
			PlayerCharacter->SpringArm->SetRelativeLocation(PlayerCharacter->GetActorLocation());
		}
		else
		{
			EdgeScroll();
			const FVector InterpolateLocation = UKismetMathLibrary::VInterpTo(PlayerCharacter->SpringArm->GetRelativeLocation(), TargetLocation, DeltaTime, CameraMoveSpeed);
			PlayerCharacter->SpringArm->SetRelativeLocation(InterpolateLocation);
		}
	}
	else
	{
		if (Map_LMB_Clicked)
		{
			const FVector InterpolateLocation = UKismetMathLibrary::VInterpTo(PlayerCharacter->SpringArm->GetRelativeLocation(), TargetLocation, DeltaTime, CameraMoveSpeed);
			PlayerCharacter->SpringArm->SetRelativeLocation(InterpolateLocation);
		}
		else
		{
			if (PlayerCharacter)
			{
				PlayerCharacter->SpringArm->SetRelativeLocation(PlayerCharacter->GetActorLocation());
			}
		}
	}

	if (PlayerCharacter)
	{
		const float InterpolateZoom = UKismetMathLibrary::FInterpTo(PlayerCharacter->SpringArm->TargetArmLength, TargetZoom, DeltaTime, 1);
		PlayerCharacter->SpringArm->TargetArmLength = InterpolateZoom;
	}
}

void ABattleFieldPlayerController::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
	if (PlayerCharacter)
		TargetZoom = PlayerCharacter->SpringArm->TargetArmLength;

	BattleFieldHUD = Cast<ABattleFieldHUD>(GetHUD());
	if (BattleFieldHUD)
	{
		ABattleFieldGameMode* BattleFieldGameMode = Cast<ABattleFieldGameMode>(GetWorld()->GetAuthGameMode());
		if(BattleFieldGameMode)
			BattleFieldHUD->SetTeamScore(BattleFieldGameMode->RedTeamScore, BattleFieldGameMode->BlueTeamScore);
	}

	ABattleFieldGameState* BGS = GetWorld()->GetGameState<ABattleFieldGameState>();
	if(BGS)
	{
		BGS->OnTeamChanged.AddUObject(this, &ABattleFieldPlayerController::ChangeControlPointUIButton);
	}

}

void ABattleFieldPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	ABattleFieldPlayerState* PS = Cast<ABattleFieldPlayerState>(PlayerState);
	if (PS)
	{
		FCharacterListRow* CharacterListRow = CharacterDataTable->FindRow<FCharacterListRow>(PS->GetPickCharacter(), TEXT(""));
		if (CharacterListRow)
		{
			PS->MaxHealth = CharacterListRow->InitMaxHealth;
			PS->Health = CharacterListRow->InitMaxHealth;
			PS->MaxMana = CharacterListRow->InitMaxMana;
			PS->Mana = CharacterListRow->InitMaxMana;
			UpdateHealthProgressBar(PS->MaxHealth, PS->Health, PS->TotalShield);
			UpdateManaProgressBar(PS->MaxMana, PS->Mana);

			PS->BaseStats.Attack = CharacterListRow->InitAttack;
			PS->BaseStats.Defense = CharacterListRow->InitDefense;
			PS->TotalStats = PS->BaseStats;
		}
	}

	if (PlayerCharacter)
	{
		PlayerCharacter->IsDied = false;
		PlayerCharacter->UpdateBar(PS->Health, 0, PS->MaxHealth);
	}
	else
	{
		PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
		PlayerCharacter->IsDied = false;
	}
	FadeOutImage(false);

	if (APlayerCharacter* player = Cast<APlayerCharacter>(InPawn))
	{
		Server_TeamSetting(player);
	}
}

void ABattleFieldPlayerController::InitPlayer()
{
	ABattleFieldPlayerState* PS = Cast<ABattleFieldPlayerState>(PlayerState);
	if (PS)
	{
		PS->Health = PS->MaxHealth;
		PS->Mana = PS->MaxMana;
		UpdateHealthProgressBar(PS->MaxHealth, PS->Health, PS->TotalShield);
		UpdateManaProgressBar(PS->MaxMana, PS->Mana);
	}

	if (PlayerCharacter)
	{
		PlayerCharacter->IsDied = false;
		PlayerCharacter->UpdateBar(PS->Health, 0, PS->MaxHealth);
	}
	else
	{
		PlayerCharacter = Cast<APlayerCharacter>(GetPawn());
		PlayerCharacter->IsDied = false;
	}
	FadeOutImage(false);
}

void ABattleFieldPlayerController::SetControllerReady(bool IsReady)
{
	IActorInterface::SetControllerReady(IsReady);

	ControllerReady = IsReady;

	Server_ControllerReady();
}

void ABattleFieldPlayerController::Server_ControllerReady_Implementation()
{
	ABattleFieldGameMode* BGM = Cast<ABattleFieldGameMode>(GetWorld()->GetAuthGameMode());

	if (BGM)
	{
		BGM->NotifyPlayerReady();
	}
}

void ABattleFieldPlayerController::FadeOutImage(bool _IsDied)
{
	BattleFieldHUD = BattleFieldHUD == nullptr ? Cast<ABattleFieldHUD>(GetHUD()) : BattleFieldHUD;
	if (BattleFieldHUD)
	{
		BattleFieldHUD->FadeOut(_IsDied);
	}
}

void ABattleFieldPlayerController::RequestRespawn(AController* ElimmedController, int32 RespawnTargetIdx)
{
	if (!PlayerCharacter->IsDied || !PlayerCharacter->GetCanRespawn())
	{
		return;
	}

	PlayerCharacter->Server_Respawn(ElimmedController, RespawnTargetIdx);
	
	if(!PlayerCharacter->GetCanRespawn())
	{
		PlayerCharacter->Server_Update_RespawnCollision();
	}
}

void ABattleFieldPlayerController::Client_RemoveLoading_Implementation()
{
	if (BattleFieldHUD)
	{
		BattleFieldHUD->PlayerOverlay->LoadingOverlay->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void ABattleFieldPlayerController::Server_TeamSetting_Implementation(APlayerCharacter* _Player)
{
	if (IActorInterface::Execute_GetETeam(_Player->GetPlayerState()) == ETeam::ET_NoTeam)
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, _Player]
			{
				Server_TeamSetting(_Player);
			}
		);

		return;
	}

	SetTeam(IActorInterface::Execute_GetETeam(_Player->GetPlayerState()));
	_Player->Server_SetTeam(IActorInterface::Execute_GetETeam(this));
	UFOWFunctionLibrary::SetDrawingEntityTeam(_Player, IActorInterface::Execute_GetTeam(this));

	FTimerDelegate HPBarDelegate;

	HPBarDelegate.BindLambda([this, _Player]()
		{
			_Player->Mul_SetHPBarColor(IActorInterface::Execute_GetETeam(this));
		});

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(
		handle,
		HPBarDelegate,
		1.f,
		false
	);

	Server_SetFOWReplicationTeam();
}

void ABattleFieldPlayerController::Server_SetFOWReplicationTeam_Implementation()
{
	AFOW_Handler* FowHandler = AFOW_Handler::GetFOWHandler(this);

	if (FowHandler)
	{
		AFOW_FogStateReplication_Server* Server = Cast<AFOW_FogStateReplication_Server>(FowHandler->GetFogStateReplication());

		if (Server)
		{
			AGameModeBase* GM = GetWorld()->GetAuthGameMode();
			AController* PC = Cast<AController>(this);
			// 로직 실행

			if (this->IsLocalController())
			{
				Server->SetPlayerTeam(IActorInterface::Execute_GetTeam(this));
				Server_ControllerReady();
			}
			else
			{
				Server->LoginPlayer(GM, PC, IActorInterface::Execute_GetTeam(this));
			}
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABattleFieldPlayerController::Server_SetFOWReplicationTeam);
	}
}

void ABattleFieldPlayerController::UpdateTeamScoreTextBlock_Implementation(int _RedTeamScore, int _BlueTeamScore)
{
	BattleFieldHUD = BattleFieldHUD == nullptr ? Cast<ABattleFieldHUD>(GetHUD()) : BattleFieldHUD;
	if (BattleFieldHUD)
	{
		BattleFieldHUD->SetTeamScore(_RedTeamScore, _BlueTeamScore);
	}
}

void ABattleFieldPlayerController::AnnounceWinner_Implementation(ETeam _Team)
{
	if (BattleFieldHUD)
	{
		BattleFieldHUD->AddWinnerAnnoucement(_Team);
	}
}

void ABattleFieldPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleFieldPlayerController, Team);
}

void ABattleFieldPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(IMC_Default, 0);
	}

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	EnhancedInputComponent->BindAction(IA_RMB, ETriggerEvent::Started, this, &ABattleFieldPlayerController::RMB_Pressed);
	EnhancedInputComponent->BindAction(IA_RMB, ETriggerEvent::Completed, this, &ABattleFieldPlayerController::RMB_Released);
	EnhancedInputComponent->BindAction(IA_RMB, ETriggerEvent::Triggered, this, &ABattleFieldPlayerController::RMB_Held);

	EnhancedInputComponent->BindAction(IA_Scroll, ETriggerEvent::Triggered, this, &ABattleFieldPlayerController::Zoom);
	EnhancedInputComponent->BindAction(IA_Y, ETriggerEvent::Started, this, &ABattleFieldPlayerController::FixCamera);

	EnhancedInputComponent->BindAction(IA_LMB, ETriggerEvent::Started, this, &ABattleFieldPlayerController::LMB_Pressed);
	EnhancedInputComponent->BindAction(IA_LMB, ETriggerEvent::Completed, this, &ABattleFieldPlayerController::LMB_Released);

	EnhancedInputComponent->BindAction(IA_A, ETriggerEvent::Started, this, &ABattleFieldPlayerController::A_Pressed);
	EnhancedInputComponent->BindAction(IA_Q, ETriggerEvent::Started, this, &ABattleFieldPlayerController::Q_Pressed);
	EnhancedInputComponent->BindAction(IA_W, ETriggerEvent::Started, this, &ABattleFieldPlayerController::W_Pressed);
	EnhancedInputComponent->BindAction(IA_E, ETriggerEvent::Started, this, &ABattleFieldPlayerController::E_Pressed);
	EnhancedInputComponent->BindAction(IA_R, ETriggerEvent::Started, this, &ABattleFieldPlayerController::R_Pressed);
	EnhancedInputComponent->BindAction(IA_M, ETriggerEvent::Started, this, &ABattleFieldPlayerController::M_Pressed);
	EnhancedInputComponent->BindAction(IA_Space, ETriggerEvent::Started, this, &ABattleFieldPlayerController::SpaceBar_Pressed);
	EnhancedInputComponent->BindAction(IA_Space, ETriggerEvent::Triggered, this, &ABattleFieldPlayerController::SpaceBar_Pressed);
	EnhancedInputComponent->BindAction(IA_Space, ETriggerEvent::Completed, this, &ABattleFieldPlayerController::SpaceBar_Released);
}

void ABattleFieldPlayerController::UpdateHealthProgressBar(float MaxHealth, float Health, float ShieldAmount)
{
	BattleFieldHUD = BattleFieldHUD == nullptr ? Cast<ABattleFieldHUD>(GetHUD()) : BattleFieldHUD;
	if (BattleFieldHUD)
	{
		BattleFieldHUD->SetHealthProgressBar(MaxHealth, Health, ShieldAmount);
	}
}

void ABattleFieldPlayerController::UpdateManaProgressBar(float MaxMana, float Mana)
{
	if (BattleFieldHUD)
	{
		BattleFieldHUD->SetManaProgressBar(MaxMana, Mana);
	}
}

void ABattleFieldPlayerController::InitCharacterImage(FCharacterListRow* CharacterListRow)
{
	BattleFieldHUD = BattleFieldHUD == nullptr ? Cast<ABattleFieldHUD>(GetHUD()) : BattleFieldHUD;
	if (BattleFieldHUD)
	{
		BattleFieldHUD->SetCharacterImage(CharacterListRow);
	}
}

void ABattleFieldPlayerController::CursorTrace()
{
	// 구조물에 가려진 땅을 클릭할 때 구조물 콜리전에 의해 땅을 인식하지 못하기 때문에 구조물의 콜리전 ECC_Visibility를 무시하도록 설정한다.
	// 미니언이나 캐릭터를 콜리전을 시키려면 ECC_Visibility를 block 처리해야함
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = CursorHit.GetActor();

	if (LastActor != ThisActor)
	{
		if (LastActor && LastActor->Implements<UActorInterface>())
			IActorInterface::Execute_UnHighlightActor(LastActor);
		if (ThisActor && ThisActor->Implements<UActorInterface>())
			IActorInterface::Execute_HighlightActor(ThisActor);
	}
}

void ABattleFieldPlayerController::AutoRun()
{
	if (!bAutoRunning || PathPointArray.IsEmpty())
		return;

	FVector WorldDirection = PathPointArray[PathPointArrayIndex] - PlayerCharacter->GetActorLocation();
	WorldDirection.Z = 0;
	PlayerCharacter->AddMovementInput(WorldDirection.GetSafeNormal());
	FVector Distance = PathPointArray[PathPointArrayIndex] - PlayerCharacter->GetActorLocation();
	Distance.Z = 0;
	if (Distance.Length() <= ArrivalRange)
	{
		if (PathPointArray.Num() - 1 == PathPointArrayIndex)
		{
			bAutoRunning = false;
			PathPointArray.Empty();
			PathPointArrayIndex = 1;
		}
		else
		{
			PathPointArrayIndex++;
		}
	}
}

void ABattleFieldPlayerController::CreateNavPath(FVector Destination)
{
	if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, PlayerCharacter->GetActorLocation(), Destination))
	{
		PathPointArray.Empty();
		if (PlayerCharacter->Ground == EGround::Ground)
		{
			for (const FVector& PointLoc : NavPath->PathPoints)
			{
				PathPointArray.Add(PointLoc);
			}
		}
		else
		{
			PathPointArray.Add(FVector::ZeroVector);
			PathPointArray.Add(Destination);
		}
	}
	bAutoRunning = true;
}

void ABattleFieldPlayerController::EdgeScroll()
{
	if (UWorld* WorldContext = GetWorld())
	{
		FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(WorldContext);
		const FVector2D ViewportSize = UWidgetLayoutLibrary::GetViewportSize(WorldContext);
		MousePosition = MousePosition * UWidgetLayoutLibrary::GetViewportScale(WorldContext);
		MousePosition.X = MousePosition.X / ViewportSize.X;
		MousePosition.Y = MousePosition.Y / ViewportSize.Y;

		if (MousePosition.X > 0.98f && MousePosition.X < 1.1f)
		{
			Right(EdgeScrollSpeed);
		}
		else if (MousePosition.X < 0.02f && MousePosition.X > -0.1f)
		{
			Right(-EdgeScrollSpeed);
		}

		if (MousePosition.Y > 0.98f && MousePosition.Y < 1.1f)
		{
			Forward(-EdgeScrollSpeed);
		}
		else if (MousePosition.Y < 0.02f && MousePosition.Y > -0.1f)
		{
			Forward(EdgeScrollSpeed);
		}
	}
}

void ABattleFieldPlayerController::Right(float AxisValue)
{
	TargetLocation = FVector3d::RightVector * AxisValue * CameraMoveSpeed + TargetLocation;

	CamLocationFix();
}

void ABattleFieldPlayerController::Forward(float AxisValue)
{
	TargetLocation = FVector3d::ForwardVector * AxisValue * CameraMoveSpeed + TargetLocation;
	
	CamLocationFix();
}

void ABattleFieldPlayerController::Zoom(const FInputActionValue& InputActionValue)
{
	const float Zoom = InputActionValue.Get<float>() * ZoomSpeed;
	TargetZoom = FMath::Clamp(TargetZoom - Zoom, MinZoom, MaxZoom);
}

void ABattleFieldPlayerController::FixCamera()
{
	if (IsFixCamera == false)
	{
		IsFixCamera = true;
	}
	else
	{
		TargetLocation = PlayerCharacter->GetActorLocation();
		IsFixCamera = false;
	}
}

void ABattleFieldPlayerController::RMB_Pressed()
{
	if(BattleFieldHUD->PlayerOverlay->IsLMB_HoldingMap)
	{
		BattleFieldHUD->PlayerOverlay->ForceLMBRelease();
		return;
	}

	if (IActorInterface::Execute_GetIsDied(PlayerCharacter)) return;
	bAutoRunning = false;
	PathPointArrayIndex = 1;

	if (PlayerCharacter)
	{
		PlayerCharacter->RMB_Pressed();
	}

	BattleFieldHUD->PlayerOverlay->ForceLMBRelease();
}

void ABattleFieldPlayerController::RMB_Released()
{
	if (IActorInterface::Execute_GetIsDied(PlayerCharacter)) return;
	CreateNavPath(CachedDestination);

	BattleFieldHUD->PlayerOverlay->ForceLMBRelease();
}

void ABattleFieldPlayerController::RMB_Held()
{
	if (IActorInterface::Execute_GetIsDied(PlayerCharacter)) return;
	if (CursorHit.bBlockingHit)
	{
		CachedDestination = CursorHit.ImpactPoint;
	}

	const FVector WorldDirection = (CachedDestination - PlayerCharacter->GetActorLocation()).GetSafeNormal();
	PlayerCharacter->AddMovementInput(WorldDirection);
}

void ABattleFieldPlayerController::LMB_Pressed()
{
	if (IActorInterface::Execute_GetIsDied(PlayerCharacter)) return;
	if (CursorHit.bBlockingHit)
	{
		CachedDestination = CursorHit.ImpactPoint;
	}
}

void ABattleFieldPlayerController::LMB_Released()
{
	if (IActorInterface::Execute_GetIsDied(PlayerCharacter)) return;
	if (ThisActor && ThisActor->Implements<UActorInterface>())
	{
		TargetActor = ThisActor;
	}
	else
	{
		TargetActor = nullptr;
	}

	if (PlayerCharacter)
	{
		PlayerCharacter->LMB_Released(CachedDestination, TargetActor);
	}

	Map_LMB_Clicked = false;
}

void ABattleFieldPlayerController::A_Pressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->A_Skill();
	}
}

void ABattleFieldPlayerController::Q_Pressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->Q_Skill();
	}
}

void ABattleFieldPlayerController::W_Pressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->W_Skill();
	}
}

void ABattleFieldPlayerController::E_Pressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->E_Skill();
	}
}

void ABattleFieldPlayerController::R_Pressed()
{
	if (PlayerCharacter)
	{
		PlayerCharacter->R_Skill();
	}
}

void ABattleFieldPlayerController::M_Pressed()
{
	if (SelectControlPointUI->GetVisibility() == ESlateVisibility::Visible)
	{
		SelectControlPointUI->SetVisibility(ESlateVisibility::Hidden);
	}
	else if (SelectControlPointUI->GetVisibility() == ESlateVisibility::Hidden)
	{
		SelectControlPointUI->SetVisibility(ESlateVisibility::Visible);
	}
}

void ABattleFieldPlayerController::SpaceBar_Pressed()
{
	Space_Pressed = true;

	if (!Map_LMB_Clicked)
	{
		TargetLocation = PlayerCharacter->GetActorLocation();
	}
}

void ABattleFieldPlayerController::SpaceBar_Released()
{
	Space_Pressed = false;
}

void ABattleFieldPlayerController::Map_LMB_Pressed(FVector2D Location)
{
	Map_LMB_Clicked = true;

	TargetLocation = FVector{ Location.X, Location.Y, PlayerCharacter->GetActorLocation().Z};
}

void ABattleFieldPlayerController::Map_LMB_Released(FVector2D Location)
{
	Map_LMB_Clicked = false;
}

void ABattleFieldPlayerController::Map_RMB_Pressed(FVector2D Location)
{
	if (IActorInterface::Execute_GetIsDied(PlayerCharacter)) return;
	PathPointArrayIndex = 1;

	CachedDestination = FVector{ Location.X, Location.Y, 0 };
	CreateNavPath(CachedDestination);
}

void ABattleFieldPlayerController::CamLocationFix()
{
	if (TargetLocation.X >= 14500)
	{
		TargetLocation.X = 14500;
	}

	if (TargetLocation.X <= -14500)
	{
		TargetLocation.X = -14500;
	}

	if (TargetLocation.Y >= 14500)
	{
		TargetLocation.Y = 14500;
	}

	if (TargetLocation.Y <= -14500)
	{
		TargetLocation.Y = -14500;
	}
}

void ABattleFieldPlayerController::SetControlPointValue(int32 _Value)
{
	if (!PlayerCharacter->IsDied)
	{
		Server_SetMinionMovePoint(_Value);
	}
}

void ABattleFieldPlayerController::ChangeControlPointUIButton()
{
	ABattleFieldGameState* BGS = GetWorld()->GetGameState<ABattleFieldGameState>();
	if (BGS)
	{
		auto list = BGS->GetControlPointsTeam();

		for (int i = 0; i < list.Num(); ++i)
		{
			SelectControlPointUI->ChangeButtonColor(i + 1,list[i]);
		}
	}
}

void ABattleFieldPlayerController::Server_SetMinionMovePoint_Implementation(int32 _Value)
{
	ABattleFieldPlayerState* playerState = GetPlayerState<ABattleFieldPlayerState>();
	playerState->SetMinionMovePoint(_Value);
}