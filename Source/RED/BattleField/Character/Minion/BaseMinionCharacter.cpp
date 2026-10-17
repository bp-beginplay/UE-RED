// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseMinionCharacter.h"
#include "MinionAIController.h"
#include "AI/NavigationSystemBase.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "RED/RED.h"
#include "FogOfWar/Public/DrawerComponent/FOW_Drawer_FOV.h"
#include "Net/UnrealNetwork.h"
#include "RED/BattleField/Component/HealthComponent.h"
#include "RED/BattleField/UserWidget/HPBarWidget.h"
#include "Kismet/GameplayStatics.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/BattleFieldPlayerState.h"

// Sets default values
ABaseMinionCharacter::ABaseMinionCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DetectionRange = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DetectionRange"));
	DetectionRange->SetupAttachment(RootComponent);

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
}

// Called when the game starts or when spawned
void ABaseMinionCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (DetectionRange)
	{
		DetectionRange->OnComponentBeginOverlap.AddDynamic(
			this,
			&ABaseMinionCharacter::OnDetectionRangeBeginOverlap
		);

		DetectionRange->OnComponentEndOverlap.AddDynamic(
			this,
			&ABaseMinionCharacter::OnDetectionRangeEndOverlap
		);
	}

	UUserWidget* RawWidget = HPWidgetComp->GetUserWidgetObject();

	UHPBarWidget* HPWidget = Cast<UHPBarWidget>(RawWidget);

	HealthComponent->OnHPChanged.AddUObject(HPWidget, &UHPBarWidget::UpdateBar);
}

void ABaseMinionCharacter::SetIsAttacking(bool _IsAttacking)
{
	IsAttacking = _IsAttacking;

	if (HasAuthority())
	{
		OnRep_IsAttacking();
	}
}

void ABaseMinionCharacter::OnRep_IsAttacking()
{
	struct FBoolParam
	{
		bool bValue; // 블루프린트의 bool 매개변수와 매칭
	};

	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		FBoolParam Params;
		Params.bValue = IsAttacking; // 넘기고 싶은 값

		UFunction* Func = AnimInst->FindFunction(FName("AnimEvent_Attaking"));
		if (Func)
		{
			AnimInst->ProcessEvent(Func, &Params);
		}
	}
}

void ABaseMinionCharacter::OnRep_MinionStats()
{
	UpdateMesh();
}

void ABaseMinionCharacter::OnRep_HP()
{
	// UI만들고 거기에 HP바 추가 해야함
}

void ABaseMinionCharacter::SetHP(float _HP)
{
	HealthComponent->SetHP(_HP);
}

float ABaseMinionCharacter::GetHP()
{
	return HealthComponent->GetHP();
}

void ABaseMinionCharacter::SetMaxHP(float _MaxHP)
{
	HealthComponent->SetMaxHP(_MaxHP);
}

float ABaseMinionCharacter::GetMaxHP()
{
	return HealthComponent->GetMaxHP();
}

void ABaseMinionCharacter::Mul_NotifyHPChanged_Implementation(float _CurHP, float _Shield, float _MaxHP)
{
	UUserWidget* RawWidget = HPWidgetComp->GetUserWidgetObject();

	UHPBarWidget* HPWidget = Cast<UHPBarWidget>(RawWidget);

	if (HPWidget)
	{
		HPWidget->UpdateBar(_CurHP, _Shield, _MaxHP);
	}
}

void ABaseMinionCharacter::UpdateMesh()
{
	if (Team == ETeam::ET_RedTeam)
	{
		if (MinionType == EMinionType::MELEE)
		{
			GetMesh()->SetSkeletalMesh(RedTeamMeleeMinionMesh.LoadSynchronous());
		}
		else if (MinionType == EMinionType::RANGE)
		{
			GetMesh()->SetSkeletalMesh(RedTeamRangeMinionMesh.LoadSynchronous());
		}
		GetMesh()->SetCustomDepthStencilValue(250);

		UMaterialInterface* Mat = LoadObject<UMaterialInterface>(
			nullptr,
			TEXT("/Game/Blueprints/BattleField/Character/Minion/RedMat.RedMat")
		);

		if (Mat)
		{
			MiniMapIcon->SetMaterial(0, Mat);
		}
	}
	else if (Team == ETeam::ET_BlueTeam)
	{
		if (MinionType == EMinionType::MELEE)
		{
			GetMesh()->SetSkeletalMesh(BlueTeamMeleeMinionMesh.LoadSynchronous());
		}
		else if (MinionType == EMinionType::RANGE)
		{
			GetMesh()->SetSkeletalMesh(BlueTeamRangeMinionMesh.LoadSynchronous());
		}
		GetMesh()->SetCustomDepthStencilValue(251);

		UMaterialInterface* Mat = LoadObject<UMaterialInterface>(
			nullptr,
			TEXT("/Game/Blueprints/BattleField/Character/Minion/BlueMat.BlueMat")
		);

		if (Mat)
		{
			MiniMapIcon->SetMaterial(0, Mat);
		}
	}
}

// Called every frame
void ABaseMinionCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseMinionCharacter::SetTargetControlPointValue(int32 _TargetControlPointValue)
{
	TargetControlPointValue = _TargetControlPointValue;
	AMinionAIController* AIController = Cast<AMinionAIController>(GetController());
	AIController->FindNewTarget(TargetControlPointValue);
}

void ABaseMinionCharacter::Server_SetFOWDrawerTeam_Implementation(int32 _Team) const
{
	FOWDrawer_FOV->SetTeamIndex(_Team);
}

void ABaseMinionCharacter::OnDetectionRangeBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, 
	bool bFromSweep, 
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(OtherActor) || OtherActor == this)
	{
		return;
	}

	if (IsValid(OtherActor))
	{
		NotifyBeginOverlap(OtherActor);
	}
}

void ABaseMinionCharacter::OnDetectionRangeEndOverlap(UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!IsValid(OtherActor) || OtherActor == this)
	{
		return;
	}

	if (IsValid(OtherActor))
	{
		NotifyEndOverlap(OtherActor);
	}
}

void ABaseMinionCharacter::NotifyBeginOverlap(AActor* _Actor)
{
	AMinionAIController* AIController = Cast<AMinionAIController>(GetController());

	AIController->BeginOverlap(_Actor);
}

void ABaseMinionCharacter::NotifyEndOverlap(AActor* _Actor)
{
	AMinionAIController* AIController = Cast<AMinionAIController>(GetController());

	AIController->EndOverlap(_Actor);
}

void ABaseMinionCharacter::NotifyOnDestroyed(AActor* _Actor)
{
	AMinionAIController* AIController = Cast<AMinionAIController>(GetController());

	AIController->OnDestroyed(_Actor);
}

float ABaseMinionCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	float Damage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	HealthComponent->HandleDamage(Damage);

	if (HealthComponent->GetHP() <= 0 && !IsDied)
	{
		Died();

		// 김동호
		CharacterAction = ECharacterAction::Die;

		if(HasAuthority())
		{
			OnRep_IsDied();
		}

		GetWorld()->GetTimerManager().SetTimer(
			GoDownHandle,
			this,
			&ABaseMinionCharacter::GoDown,
			0.02f,
			true,
			3.0f
		);
	}
	return Damage;
}

void ABaseMinionCharacter::GoDown()
{
	SetActorLocation(GetActorLocation() - FVector(0,0,2));

	if (GetActorLocation().Z <= -200)
	{
		GetWorld()->GetTimerManager().ClearTimer(GoDownHandle);
		Destroy();
	}
}

void ABaseMinionCharacter::Died()
{
	IsDied = true;

	// 포인트 분배
	float EnemyPlayerCount = 0.f;
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
			ABattleFieldPlayerState* PS = Cast<ABattleFieldPlayerState>(iter->GetPlayerState());
			PS->Server_AddPoint(3 / EnemyPlayerCount);
		}
	}


	AMinionAIController* AIController = Cast<AMinionAIController>(GetController());
	if (AIController)
	{
		AIController->Died();
	}
}

void ABaseMinionCharacter::OnRep_IsDied()
{
	if (UAnimInstance* AnimInst = GetMesh()->GetAnimInstance())
	{
		// 블루프린트에서 만든 커스텀 이벤트를 이름으로 호출
		// (함수 이름이 AnimEvent_Died라고 가정)
		UFunction* Func = AnimInst->FindFunction(FName("AnimEvent_Died"));
		if (Func)
		{
			AnimInst->ProcessEvent(Func, nullptr);
		}
	}

	Super::OnRep_IsDied();
}

void ABaseMinionCharacter::UpdateCollision()
{
	Super::UpdateCollision();

	DetectionRange->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABaseMinionCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseMinionCharacter, IsAttacking);
	DOREPLIFETIME(ABaseMinionCharacter, MinionType);
}