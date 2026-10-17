// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "Components/CapsuleComponent.h"
#include "Entity/Visibility/FOW_VisibilityE_Component.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "RED/BattleField/BattleFieldPlayerState.h"

#include "FogOfWar/Public/DrawerComponent/FOW_Drawer_FOV.h"
#include "RED/RED.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/BattleField/UserWidget/HPBarWidget.h"
#include "Tools/FunctionLibrairy/FOWFunctionLibrary.h"

ABaseCharacter::ABaseCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	FOWDrawer_FOV = CreateDefaultSubobject<UFOW_Drawer_FOV>(TEXT("FOW_Drawer_FOV_Comp"));
	FOWDrawer_FOV->SetDrawerRadius(1500);

	GetCapsuleComponent()->SetCollisionProfileName(TEXT("CapsuleCollision"));
	GetMesh()->SetCollisionProfileName(TEXT("CharacterCollision"));
	GetMesh()->SetGenerateOverlapEvents(true);

	FOW_VisibilityE = CreateDefaultSubobject<UFOW_VisibilityE_Component>(TEXT("FOW_Visibility_Comp"));
	FOW_VisibilityE->DisableRenderOutSight = true;

	HPWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("HPWidgetComponent"));
	HPWidgetComp->SetupAttachment(RootComponent);
	HPWidgetComp->SetHiddenInSceneCapture(true);

	static ConstructorHelpers::FClassFinder<UUserWidget> HPWidgetAsset(TEXT("WidgetBlueprint'/Game/Blueprints/BattleField/UserWidget/BP_HPBar.BP_HPBar_C'"));

	if (HPWidgetAsset.Succeeded())
	{
		HPWidgetComp->SetWidgetClass(HPWidgetAsset.Class);

		HPWidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		//HPWidgetComp->SetDrawAtDesiredSize(true);
		HPWidgetComp->SetDrawSize(FVector2D{ 150,25 });
		HPWidgetComp->SetPivot(FVector2D{0.5,5});
	}

	MiniMapIcon = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MiniMapIcon"));
	MiniMapIcon->AttachToComponent(
		RootComponent,
		FAttachmentTransformRules::KeepRelativeTransform);
	MiniMapIcon->SetUsingAbsoluteRotation(true);

	MiniMapIcon->SetRelativeLocation(FVector{ 0,0,2000 });
	MiniMapIcon->SetWorldRotation(FRotator{ 0,90,0 });
	MiniMapIcon->SetRelativeScale3D(FVector{ 15,15,15 });
	MiniMapIcon->SetCollisionProfileName(TEXT("NoCollision"));
	MiniMapIcon->SetVisibleInSceneCaptureOnly(true);
	MiniMapIcon->SetCastShadow(false);

	static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Plane.Plane"));

	if (MeshAsset.Succeeded())
	{
		MiniMapIcon->SetStaticMesh(MeshAsset.Object);
	}

	SetNetUpdateFrequency(66.f);
	SetMinNetUpdateFrequency(33.f);
	SetNetCullDistanceSquared(10000000000.f);
}

void ABaseCharacter::OnRep_IsDied()
{
	if(IsDied)
	{
		UpdateCollision();
		HPWidgetComp->SetVisibility(false);
		ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(Controller);
		if (PC)
		{
			ABattleFieldPlayerState* BPS = PC->GetPlayerState<ABattleFieldPlayerState>();

			if (BPS)
			{
				BPS->ClearBuff();
			}
			PC->FadeOutImage(IsDied);
		}
	}
	else
	{
		Update_RespawnCollision();

		ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(Controller);
		if (PC)
		{
			ABattleFieldPlayerState* BPS = PC->GetPlayerState<ABattleFieldPlayerState>();

			if (BPS)
			{
				BPS->ClearBuff();
			}
			PC->FadeOutImage(IsDied);
		}
	}
}

void ABaseCharacter::UpdateCollision()
{
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->GravityScale = 0;
	MiniMapIcon->SetHiddenInSceneCapture(true);

	FTimerHandle handle;
	GetWorld()->GetTimerManager().SetTimer(
		handle,
		this,
		&ABaseCharacter::SetDrawerTeam_OnDied,
		1.f,
		false
	);
}

void ABaseCharacter::Update_RespawnCollision()
{
	HPWidgetComp->SetVisibility(true);

	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	GetCharacterMovement()->GravityScale = 1.0f;
	MiniMapIcon->SetHiddenInSceneCapture(false);
	
	UFOWFunctionLibrary::SetDrawingEntityTeam(this, IActorInterface::Execute_GetTeam(this));
}

void ABaseCharacter::Attack(FRotator _TargetRotation)
{
	if (IsLocallyControlled())
	{
		PlayAttackMontage(_TargetRotation);
	}
	Server_Attack(_TargetRotation);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	//switch (Team)
	//{
	//case ETeam::ET_RedTeam:
	//	FOWDrawer_FOV->SetTeamIndex(0);
	//	break;
	//case ETeam::ET_BlueTeam:
	//	FOWDrawer_FOV->SetTeamIndex(1);
	//	break;
	//case ETeam::ET_NoTeam:
	//	FOWDrawer_FOV->SetTeamIndex(2);
	//	break;
	//default:
	//	break;
	//}

}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, Team);
	DOREPLIFETIME(ABaseCharacter, IsDied);
	DOREPLIFETIME(ABaseCharacter, CharacterAction);
	DOREPLIFETIME(ABaseCharacter, Ground);
}

float ABaseCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}

void ABaseCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel5);
}

void ABaseCharacter::OnRep_Ground()
{

}

void ABaseCharacter::Server_Attack_Implementation(FRotator _TargetRotation)
{
	NetMulti_Attack(_TargetRotation);
}

void ABaseCharacter::NetMulti_Attack_Implementation(FRotator _TargetRotation)
{
	if (IsLocallyControlled()) return;
	PlayAttackMontage(_TargetRotation);
}

void ABaseCharacter::PlayAttackMontage(FRotator _TargetRotation)
{
	SetActorRotation(_TargetRotation);

	if (CharacterAction == ECharacterAction::Attack) return;
	CharacterAction = ECharacterAction::Attack;

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AttackMontage)
	{
		// Montage_Play의 매개변수 InPlayRate가 공격속도
		AnimInstance->Montage_Play(AttackMontage);
	}
}

void ABaseCharacter::EndAttackMontage()
{
	CharacterAction = ECharacterAction::Idle;
}

void ABaseCharacter::SetDrawerTeam_OnDied()
{
	UFOWFunctionLibrary::SetDrawingEntityTeam(this, 2);
}

void ABaseCharacter::StopMontage()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->StopAllMontages(0.0f);
	}
	CharacterAction = ECharacterAction::Idle;
}

void ABaseCharacter::HighlightActor_Implementation()
{
	// 뎁스 스텐실 값을 변경하기 위해 프로젝트 세팅 - 엔진 - 렌더링 - 포스트 프로세싱 - 커스텀 뎁스-스텐실 패스 -> Enabled with Stencil로 변경
	switch (Team)
	{
	case ETeam::ET_RedTeam:
		GetMesh()->SetCustomDepthStencilValue(250);
		break;
	case ETeam::ET_BlueTeam:
		GetMesh()->SetCustomDepthStencilValue(251);
		break;
	case ETeam::ET_NoTeam:
		GetMesh()->SetCustomDepthStencilValue(252);
		break;
	default:
		break;
	}
	GetMesh()->SetRenderCustomDepth(true);
}

void ABaseCharacter::UnHighlightActor_Implementation()
{
	GetMesh()->SetRenderCustomDepth(false);
}

void ABaseCharacter::Server_SetTeam_Implementation(ETeam _Team)
{
	Team = _Team;
}

int32 ABaseCharacter::GetVisibility_Implementation() const
{
	return FOW_VisibilityE->GetVisibilityState();
}

bool ABaseCharacter::IsEnemy(AActor* _OtherActor) const
{
	if (!IsValid(_OtherActor))
	{
		return false;
	}

	if (!_OtherActor->Implements<UActorInterface>())
	{
		return false;
	}

	const int32 OtherTeam = IActorInterface::Execute_GetTeam(_OtherActor);

	return OtherTeam != static_cast<int32>(Team);
}

void ABaseCharacter::Mul_SetHPBarColor_Implementation(ETeam _Team)
{
	UE_LOG(LogTemp, Warning, TEXT("Mul_SetHPBarColor"));
	UE_LOG(LogTemp, Error, TEXT("Name : %s"), *this->GetName());
	if (_Team == ETeam::ET_BlueTeam)
	{
		UE_LOG(LogTemp, Warning, TEXT("BlueTeam"));
	}
	else if (_Team == ETeam::ET_RedTeam)
	{
		UE_LOG(LogTemp, Warning, TEXT("RedTeam"));
	}

	UUserWidget* Widget = HPWidgetComp->GetUserWidgetObject();
	if (!Widget)
	{
		UE_LOG(LogTemp, Error, TEXT("No Widget"));
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, _Team]
			{
				Mul_SetHPBarColor(_Team);
			});
		return;
	}

	UHPBarWidget* HPWidget = Cast<UHPBarWidget>(Widget);
	if (!HPWidget || !HPWidget->HealthBar)
	{
		UE_LOG(LogTemp, Error, TEXT("No HPBar"));
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, _Team]
			{
				Mul_SetHPBarColor(_Team);
			});
		return;
	}

	if (_Team == ETeam::ET_BlueTeam)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set ET_BlueTeam"));
		HPWidget->HealthBar->SetFillColorAndOpacity(FLinearColor::Blue);
	}
	else if (_Team == ETeam::ET_RedTeam)
	{
		UE_LOG(LogTemp, Warning, TEXT("Set ET_RedTeam"));
		HPWidget->HealthBar->SetFillColorAndOpacity(FLinearColor::Red);
	}
}