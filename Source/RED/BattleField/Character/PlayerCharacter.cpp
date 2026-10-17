// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Components/Image.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

#include "RED/BattleField/BattleFieldGameMode.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/BattleFieldPlayerState.h"
#include "RED/BattleField/Actor/Projectile/HomingProjectile.h"
#include "RED/BattleField/Component/AttackComponent.h"
#include "RED/BattleField/Character/Human/CheonIJin/CheonIJin.h"

#include "FogOfWar/Public/DrawerComponent/FOW_Drawer_FOV.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Net/UnrealNetwork.h"
#include "RED/BattleField/BattleFieldHUD.h"
#include "RED/BattleField/UserWidget/HPBarWidget.h"
#include "RED/BattleField/UserWidget/PlayerOverlay.h"
#include "RED/Interface/SelectControlZoneUI.h"

APlayerCharacter::APlayerCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(84.f, 192.0f);
	GetCapsuleComponent()->SetHiddenInGame(true);
	GetCapsuleComponent()->SetHiddenInSceneCapture(true);

	GetMesh()->SetRelativeLocation(FVector{ 0,0,-190 });
	GetMesh()->SetRelativeRotation(FRotator{ 0,-90,0 });
	GetMesh()->SetRelativeScale3D(FVector{ 2,2,2 });
	GetMesh()->SetHiddenInSceneCapture(true);

	AttackRangeCollision = CreateDefaultSubobject<USphereComponent>(TEXT("AttackRangeCollision"));
	AttackRangeCollision->SetupAttachment(GetRootComponent());
	AttackRangeCollision->InitSphereRadius(100.f);
	AttackRangeCollision->SetHiddenInGame(true);
	AttackRangeCollision->SetHiddenInSceneCapture(true);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->MaxWalkSpeed = 1000.f;

	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = 2000.f;
	SpringArm->SetRelativeRotation(FRotator(-70.f, 0.f, 0.f));
	SpringArm->bDoCollisionTest = false;

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
	Camera->bUsePawnControlRotation = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;

	FOWDrawer_FOV->SetDrawerRadius(1500);

	AttackComponent = CreateDefaultSubobject<UAttackComponent>(TEXT("AttackComponent"));
	AttackComponent->SetIsReplicated(true);

	bAlwaysRelevant = true;

	MiniMapCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCaptureComponent"));
	MiniMapCapture->SetupAttachment(nullptr);
	MiniMapCapture->SetUsingAbsoluteLocation(true);
	MiniMapCapture->SetUsingAbsoluteRotation(true);
	MiniMapCapture->ProjectionType = ECameraProjectionMode::Orthographic;
	MiniMapCapture->OrthoWidth = 30000.f;
	MiniMapCapture->SetWorldRotation(FRotator{ -90.f,0.f,0.f });

	MiniMapCapture->CaptureSource = SCS_BaseColor;
	//MiniMapCapture->CaptureSource = SCS_SceneColorHDR;
	//MiniMapCapture->CaptureSource = SCS_SceneColorHDRNoAlpha;

	MiniMapCapture->ShowFlags.SetParticles(false);
	MiniMapCapture->ShowFlags.SetNiagara(false);
	MiniMapCapture->ShowFlags.SetPostProcessing(false);
	MiniMapCapture->ShowFlags.SetLighting(true);

	MiniMapCapture->PostProcessSettings.bOverride_AutoExposureMinBrightness = true;
	MiniMapCapture->PostProcessSettings.bOverride_AutoExposureMaxBrightness = true;
	MiniMapCapture->PostProcessSettings.AutoExposureMinBrightness = 1.0f;
	MiniMapCapture->PostProcessSettings.AutoExposureMaxBrightness = 1.0f;

	MiniMapCapture->bCaptureEveryFrame = true;

	SkillRange1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillRange1"));
	SkillRange1->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	SkillRange2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SkillRange2"));
	SkillRange2->AttachToComponent(SkillRange1, FAttachmentTransformRules::KeepRelativeTransform);

	SkillRange1->SetRelativeLocation(FVector{ 0,0,-120 });
	SkillRange1->SetRelativeRotation(FRotator{ 0,0,0 });
	SkillRange1->SetRelativeScale3D(FVector{ 1,1,1 });
	SkillRange1->SetCollisionProfileName(TEXT("NoCollision"));
	SkillRange1->SetCastShadow(false);
	SkillRange1->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset1(TEXT("/ControlRig/Controls/ControlRig_Circle_solid.ControlRig_Circle_solid"));

	if (MeshAsset1.Succeeded())
	{
		SkillRange1->SetStaticMesh(MeshAsset1.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset1(TEXT("/Game/Test/SkyBlue.SkyBlue"));

	if (MaterialAsset1.Succeeded())
	{
		SkillRange1->SetMaterial(0, MaterialAsset1.Object);
	}

	SkillRange2->SetRelativeLocation(FVector{ 0,0,0 });
	SkillRange2->SetRelativeRotation(FRotator{ 0,0,0 });
	SkillRange2->SetRelativeScale3D(FVector{ 1,1,1 });
	SkillRange2->SetCollisionProfileName(TEXT("NoCollision"));
	SkillRange2->SetCastShadow(false);
	SkillRange2->SetHiddenInGame(true);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset2(TEXT("/ControlRig/Controls/ControlRig_Circle_3mm.ControlRig_Circle_3mm"));

	if (MeshAsset2.Succeeded())
	{
		SkillRange2->SetStaticMesh(MeshAsset2.Object);
	}

	static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset2(TEXT("/Game/Test/Blue.Blue"));

	if (MaterialAsset2.Succeeded())
	{
		SkillRange2->SetMaterial(0, MaterialAsset2.Object);
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	AttackRangeCollision->OnComponentBeginOverlap.AddDynamic(this, &APlayerCharacter::BeginOverlap);
	AttackRangeCollision->OnComponentEndOverlap.AddDynamic(this, &APlayerCharacter::EndOverlap);

	ABattleFieldPlayerState* PS = Cast<ABattleFieldPlayerState>(GetPlayerState());
	if (IsValid(PS))
	{
		int32 TeamIdx = IActorInterface::Execute_GetTeam(PS);
		FOWDrawer_FOV->SetTeamIndex(TeamIdx);
	}

	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &APlayerCharacter::ReceiveDamage);
	}

	if (AttackComponent)
	{
		AttackComponent->PlayerCharacter = this;
		AttackRangeCollision->SetSphereRadius(AttackComponent->ArrangeRadius);
	}

	UUserWidget* RawWidget = HPWidgetComp->GetUserWidgetObject();

	UHPBarWidget* HPWidget = Cast<UHPBarWidget>(RawWidget);
	if (HPWidget)
	{
		bWidgetReady = true;
		if (IsValid(PS))
		{
			UpdateBar(PS->Health, 0, PS->MaxHealth);
		}
	}

	if (IsLocallyControlled())
	{
		MinimapRT = UKismetRenderingLibrary::CreateRenderTarget2D(
			GetWorld(),
			512,
			512
		);

		MiniMapCapture->TextureTarget = MinimapRT;

		MiniMapCapture->SetWorldLocation(FVector{ 0,0,10000 });
		MiniMapCapture->SetWorldRotation(FRotator{ -90.f,0.f,0.f });

		//GetWorld()->GetTimerManager().SetTimerForNextTick(this, &APlayerCharacter::InitMinimapUI);
		FTimerHandle h;
		GetWorld()->GetTimerManager().SetTimer(h, 
			this, 
			&APlayerCharacter::InitMinimapUI,
			1.f,
			false);
	}
}

void APlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	BattleFieldPlayerState = Cast<ABattleFieldPlayerState>(GetPlayerState());

	if (BattleFieldPlayerState)
	{
		bPlayerStateReady = true;
		// 임시해결책
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &APlayerCharacter::UpdateBar1);
		//UpdateBar(BattleFieldPlayerState->Health, 0, BattleFieldPlayerState->MaxHealth);
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CharacterAction == ECharacterAction::Damaged)
	{
		SafeTime -= DeltaTime;
		if (SafeTime <= 0)
		{
			CharacterAction = ECharacterAction::Idle;
		}
	}
	else
	{
		SafeTime = 5.f;
	}
}

void APlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	BattleFieldPlayerState = Cast<ABattleFieldPlayerState>(GetPlayerState());
	if (IsValid(BattleFieldPlayerState))
	{
		bPlayerStateReady = true;
		UpdateBar(BattleFieldPlayerState->Health, 0, BattleFieldPlayerState->MaxHealth);
	}
}

void APlayerCharacter::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *OtherActor->GetName());

	if (!OtherActor->Implements<UActorInterface>()) return;

	if (IsEnemy(OtherActor))
	{
		TargetActorArray.Add(OtherActor);
	}
}

void APlayerCharacter::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	TargetActorArray.Remove(OtherActor);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	if (IsDied) return 0.0f;
	float damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);;

	ABattleFieldPlayerState* PS = Cast<ABattleFieldPlayerState>(GetPlayerState());
	if (PS)
	{
		PS->CalculateHealth(damage);
		if (PS->Health <= 0)
		{
			IsDied = true;
			GetCharacterMovement()->MovementMode = EMovementMode::MOVE_Walking;
			GetCharacterMovement()->SetPlaneConstraintEnabled(false);
			Ground = EGround::Ground;

			if (HasAuthority())
			{
				OnRep_IsDied();
			}

			int EnemyPlayerCount = 0;
			TArray<AActor*> PlayerArray;
			TArray<APlayerCharacter*> EnemyPlayerArray;
			UGameplayStatics::GetAllActorsOfClass(this, APlayerCharacter::StaticClass(), PlayerArray);
			for (auto& iter : PlayerArray)
			{
				if (IsEnemy(iter))
				{
					FVector DistanceVector = GetActorLocation() - iter->GetActorLocation();
					DistanceVector.Z = 0.f;
					if (DistanceVector.Length() <= 1000.f)
					{
						EnemyPlayerArray.Add(Cast<APlayerCharacter>(iter));
						EnemyPlayerCount++;
					}
				}
			}
			if (EnemyPlayerCount != 0)
			{
				for (auto& iter : EnemyPlayerArray)
				{
					ABattleFieldPlayerState* EnemyPS = Cast<ABattleFieldPlayerState>(iter->GetPlayerState());
					EnemyPS->Server_AddPoint(3 / EnemyPlayerCount);
				}
			}

			FTimerHandle TimerHandle;
			/*GetWorldTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([this]()
					{
						Respawn(RedCircleComponent, ETeam::ET_RedTeam);
					}),
				5.0f,
				false
			);*/

			GetWorldTimerManager().SetTimer(
				TimerHandle,
				this,
				&APlayerCharacter::SetCanRespawnTrue,
				5.0f,
				false
			);
		}
	}

	return damage;
}

void APlayerCharacter::Server_Respawn_Implementation(AController* ElimmedController, int32 RespawnTargetIdx)
{
	ABattleFieldGameMode* BattleFieldGameMode = GetWorld()->GetAuthGameMode<ABattleFieldGameMode>();
	if (BattleFieldGameMode)
	{
		BattleFieldGameMode->RequestRespawn(this, ElimmedController, RespawnTargetIdx);
	}
}

void APlayerCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	CharacterAction = ECharacterAction::Damaged;
	SafeTime = 5.f;
}

void APlayerCharacter::SetCanRespawnTrue()
{
	SetCanRespawn(true);
	OnRep_CanRespawn();
}

void APlayerCharacter::OnRep_CanRespawn()
{
	ABattleFieldPlayerController* BPS = Cast<ABattleFieldPlayerController>(GetController());
	if (BPS && BPS->SelectControlPointUI)
	{
		BPS->SelectControlPointUI->SetVisibility(ESlateVisibility::Visible);
	}

	if (!CanRespawn)
	{
		Server_Update_RespawnCollision();
	}
}

void APlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APlayerCharacter, CanRespawn);
}

void APlayerCharacter::InitMinimapUI()
{
	ABattleFieldPlayerController* BPC = Cast<ABattleFieldPlayerController>(GetController());

	if (BPC)
	{
		if (BPC->BattleFieldHUD)
		{
			BPC->BattleFieldHUD->PlayerOverlay->CreateUIMat(MinimapRT);
		}
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimerForNextTick(this, &APlayerCharacter::InitMinimapUI);
	}
}

void APlayerCharacter::UpdateBar(float _CurHP, float _ShieldAmount, float _MaxHP)
{
	if (!bWidgetReady || !bPlayerStateReady)
	{
		return;
	}

	ABattleFieldPlayerState* PS = GetPlayerState<ABattleFieldPlayerState>();
	if (!IsValid(PS)) return;

	UUserWidget* RawWidget = HPWidgetComp->GetUserWidgetObject();

	UHPBarWidget* HPWidget = Cast<UHPBarWidget>(RawWidget);
	if(HPWidget)
	{
		HPWidget->UpdateBar(_CurHP, _ShieldAmount, _MaxHP);
	}
}

void APlayerCharacter::UpdateBar1()
{
	UpdateBar(BattleFieldPlayerState->Health, 0, BattleFieldPlayerState->MaxHealth);
}

void APlayerCharacter::Server_Update_RespawnCollision_Implementation()
{
	IsDied = false;

	Update_RespawnCollision();
}

void APlayerCharacter::LifeSteal(float TotalDamage)
{
	BattleFieldPlayerState->HealHealth(TotalDamage * BattleFieldPlayerState->TotalStats.LifeSteel);
}

void APlayerCharacter::A_Skill()
{
	AttackComponent->SkillSetTarget();

	float Scale = AttackComponent->ArrangeRadius / 50.f;
	SkillRange1->SetHiddenInGame(false);
	SkillRange2->SetHiddenInGame(false);
	SkillRange1->SetRelativeScale3D(FVector{ Scale ,Scale ,Scale });
	AttackComponent->SkillSetTarget();
}

void APlayerCharacter::Q_Skill()
{
	if (IsDied) return;
}

void APlayerCharacter::W_Skill()
{
	if (IsDied) return;
}

void APlayerCharacter::E_Skill()
{
	if (IsDied) return;
}

void APlayerCharacter::R_Skill()
{
	if (IsDied) return;
}

void APlayerCharacter::LMB_Pressed()
{
	if (IsDied) return;
}

void APlayerCharacter::LMB_Released(FVector _TargetPosition, AActor* _Actor)
{
	if (IsDied) return;
	AttackComponent->SkillStart(_TargetPosition);
}

void APlayerCharacter::RMB_Pressed()
{
	if (IsDied) return;
	AttackComponent->Server_SetbAutoRun(false);
	AttackComponent->Server_SetTargetActor(nullptr);
}

void APlayerCharacter::RMB_Released()
{
	if (IsDied) return;
}