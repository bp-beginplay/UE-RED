// Fill out your copyright notice in the Description page of Project Settings.

#include "BattleFieldPlayerState.h"

#include "BattleFieldGameMode.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Component/SkillComponent.h"
#include "../Info/StructInfo.h"
#include "Components/TextBlock.h"

#include "Actor/ControlPoint/ControlPointBaseActor.h"
#include "Character/Devil/Yaco/YacoSkill_Q.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "RED/BattleField/Character/PlayerCharacter.h"
#include "RED/BattleField/BattleFieldPlayerController.h"
#include "RED/Info/CharacterListRow.h"
#include "RED/RED.h"
#include "RED/Red_GameInstance.h"
#include "RED/Info/RedGamePlayTags.h"
#include "RED/Interface/SelectControlZoneUI.h"

ABattleFieldPlayerState::ABattleFieldPlayerState()
{
	SetNetUpdateFrequency(60.f);
	SetMinNetUpdateFrequency(30.f);
	SetReplicates(true);
}

void ABattleFieldPlayerState::CalculateHealth(float Damage)
{
	float RemainingDamage = Damage;

	for (int i = 0; i < ActiveShields.Num(); ++i)
	{
		FShieldData& Shield = ActiveShields[i];

		float Used = FMath::Min(Shield.ShieldAmount, RemainingDamage);
		Shield.ShieldAmount -= Used;
		RemainingDamage -= Used;

		if (Shield.ShieldAmount <= 0.f)
		{
			if (Shield.SpawnedEffect)
			{
				Shield.SpawnedEffect->Destroy();
				Shield.SpawnedEffect = nullptr;
			}

			GetWorld()->GetTimerManager().ClearTimer(Shield.TimerHandle);

			ActiveShields.RemoveAt(i);
			--i;
		}

		if (RemainingDamage <= 0.f)
		{
			break;
		}
	}

	TotalShield = 0.f;

	for (auto& Shield : ActiveShields)
	{
		TotalShield += Shield.ShieldAmount;
	}

	Health = FMath::Clamp(Health - RemainingDamage, 0.f, MaxHealth);
	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		PC->UpdateHealthProgressBar(MaxHealth, Health, TotalShield);
	}

	OnRep_Health();
}

void ABattleFieldPlayerState::HealHealth(float HealAmount)
{
	Health += HealAmount;

	float OverFlowHealth = 0.f;

	if (Health >= MaxHealth)
	{
		OverFlowHealth = Health - MaxHealth;
		Health = MaxHealth;
	}

	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		PC->UpdateHealthProgressBar(MaxHealth, Health, TotalShield);
	}

	if (OnHpChanged.IsBound() && OverFlowHealth > KINDA_SMALL_NUMBER)
	{
		OnHpChanged.Broadcast(OverFlowHealth);
	}

	OnRep_Health();
}

void ABattleFieldPlayerState::Server_UpgradeMaxHealth_Implementation(float _MaxHealth)
{
	if (Point <= 0.f) return;
	MaxHealth += _MaxHealth;
	Point -= 1.f;
}

void ABattleFieldPlayerState::Server_AddPoint_Implementation(float _Point)
{
	Point += _Point;
	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		FString PointText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Point));
		if(PC->SelectControlPointUI)
			PC->SelectControlPointUI->RemainPointTextBlock->SetText(FText::FromString(PointText));
	}
}

void ABattleFieldPlayerState::Server_AddBaseState_Implementation(float _Attack)
{
	if (Point <= 0.f) return;
	BaseStats.Attack += _Attack;
	Point -= 1.f;
}

void ABattleFieldPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABattleFieldPlayerState, Team);
	DOREPLIFETIME(ABattleFieldPlayerState, CharacterName);
	DOREPLIFETIME(ABattleFieldPlayerState, MaxHealth);
	DOREPLIFETIME(ABattleFieldPlayerState, MaxMana);
	DOREPLIFETIME(ABattleFieldPlayerState, Health);
	DOREPLIFETIME(ABattleFieldPlayerState, Mana);
	DOREPLIFETIME(ABattleFieldPlayerState, BaseStats);
	DOREPLIFETIME(ABattleFieldPlayerState, TotalStats);
	DOREPLIFETIME(ABattleFieldPlayerState, ActiveShields);
	DOREPLIFETIME(ABattleFieldPlayerState, TotalShield);
	DOREPLIFETIME(ABattleFieldPlayerState, Point);
}

void ABattleFieldPlayerState::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ABattleFieldPlayerState::SetSpawnSlot);
	
	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		FCharacterListRow* CharacterListRow = PC->CharacterDataTable->FindRow<FCharacterListRow>(CharacterName, TEXT(""));
		if (CharacterListRow)
		{
			PC->InitCharacterImage(CharacterListRow);
		}
	}
}

void ABattleFieldPlayerState::OnRep_Health()
{
 	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		PC->UpdateHealthProgressBar(MaxHealth, Health, TotalShield);
	}

	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerChar->UpdateBar(Health, TotalShield, MaxHealth);
	}
}

void ABattleFieldPlayerState::OnRep_Mana()
{
	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		PC->UpdateManaProgressBar(MaxMana, Mana);
	}
}

void ABattleFieldPlayerState::OnRep_Shield()
{
	TotalShield = 0.f;

	for (auto& Shield : ActiveShields)
	{
		TotalShield += Shield.ShieldAmount;
	}
	
	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		PC->UpdateHealthProgressBar(MaxHealth, Health, TotalShield);
	}

	if (APlayerCharacter* PlayerChar = Cast<APlayerCharacter>(GetPawn()))
	{
		PlayerChar->UpdateBar(Health, TotalShield, MaxHealth);
	}
}

void ABattleFieldPlayerState::OnRep_Point()
{
	ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(GetOwningController());
	if (PC)
	{
		FString PointText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Point));
		PC->SelectControlPointUI->RemainPointTextBlock->SetText(FText::FromString(PointText));
	}
}

void ABattleFieldPlayerState::CopyProperties(class APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ABattleFieldPlayerState* MyPlayerState = Cast<ABattleFieldPlayerState>(PlayerState);
	if (MyPlayerState)
	{
		MyPlayerState->CharacterName = CharacterName;
		MyPlayerState->Team = Team;
	}
}

void ABattleFieldPlayerState::SetSpawnSlot()
{
	if (GetUniqueId().IsValid())
	{
		OwnedSlot = Cast<URed_GameInstance>(GetGameInstance())->GetMinionSlot(GetUniqueId());
	}
	else
	{
		GetWorldTimerManager().SetTimerForNextTick(this, &ABattleFieldPlayerState::SetSpawnSlot);
	}
}

void ABattleFieldPlayerState::SetTeam_int(int32 _TeamIdx)
{
	switch (_TeamIdx)
	{
	case 0:
		Team = ETeam::ET_RedTeam;
		break;
	case 1:
		Team = ETeam::ET_BlueTeam;
		break;
	default:
		break;
	}
}

void ABattleFieldPlayerState::SetMinionMovePoint(int32 _Value)
{
	ABattleFieldGameMode* GameMode =
		Cast<ABattleFieldGameMode>(UGameplayStatics::GetGameMode(GetWorld()));

	check(GameMode);

	ControlPointList = GameMode->GetControlPointList();

	for (auto ControlPoint : ControlPointList)
	{
		ControlPoint->SetMinionMovePoint(Team, OwnedSlot, _Value);
	}
}

void ABattleFieldPlayerState::AddBuff(const FBuffData& Buff)
{
	// 버프 자체를 따로 관리하는 컴포넌트를 만들거나 할 수도 있음 - 나중에
	// 이렇게 만들면 결국 지속시간이 갱신되야하는 경우는 처리 불가능 - 처리중
	// 근데 지속시간이 갱신되는 경우에 맞춰서 만들면 스택당 개별 지속시간이 있을경우 처리 불가능
	// 그러면 버프의 타입을 만들고 그 타입에 따라서 타이머를 조절하게 한다면??

	// 버프마다의 태그로 구분
	// 같은 버프인지는 여기서 구분
	TagCount.FindOrAdd(Buff.BuffTag)++;

	FBuffData Temp;
	FBuffData& NewBuff = Temp;

	// 버프 추가했는데 해당 버프의 최대 개수가 넘어가면 최대개수로 다시 설정
	// 만약 스택당 시간이 개별로 돌아간다면 최대 개수 제한은 의미가 없을것.
	int* CurCount = TagCount.Find(Buff.BuffTag);

	// 스택이 존재하고 갱신되는 버프라면
	if (Buff.BuffType == EBuffType::REFRESHONSTACK)
	{
		// 이미 있는 버프라면
		if (*CurCount > 1)
		{
			for (auto& RefreshBuff : ActiveBuffs)
			{
				// 해당 버프의 태그를 찾고
				if (RefreshBuff.BuffTag == Buff.BuffTag)
				{
					// 타이머 클리어 시키고
					GetWorld()->GetTimerManager().ClearTimer(RefreshBuff.TimerHandle);
					// 이미 있는 버프를 새로운 버프로 취급
					NewBuff = RefreshBuff;
					break;
				}
			}
		}
		// 없는 버프라면
		else
		{
			// 버프를 배열에 추가
			ActiveBuffs.Add(Buff);
			NewBuff = ActiveBuffs.Last();
		}
	}
	// 버프 시간이 갱신이 아닌 개별 시간이라면
	else if (Buff.BuffType == EBuffType::PERSTACK)
	{
		// 그냥 바로 추가해서 따로 타이머 돌리기
		ActiveBuffs.Add(Buff);
		NewBuff = ActiveBuffs.Last();
	}
	
	// 버프 스택수가 최대치를 넘어서면 최대치로 갱신
	if (*CurCount > Buff.MaxCount)
	{
		*CurCount = Buff.MaxCount;
	}

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, BuffTag = NewBuff.BuffTag]()
		{
			RemoveBuff(BuffTag);
		});

	// 지속시간이 0이면 영구버프
	if (NewBuff.Duration > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			NewBuff.TimerHandle,
			Delegate,
			NewBuff.Duration,
			false
		);
	}
	else
	{
		// 영구버프는 나중에 구현
	}
}

void ABattleFieldPlayerState::AddDeBuff(const FBuffData& Buff)
{
	TagCount.FindOrAdd(Buff.BuffTag)++;

	FBuffData Temp;
	FBuffData& NewBuff = Temp;

	int* CurCount = TagCount.Find(Buff.BuffTag);

	// 스택이 존재하고 갱신되는 버프라면
	if (Buff.BuffType == EBuffType::REFRESHONSTACK)
	{
		// 이미 있는 버프라면
		if (*CurCount > 1)
		{
			for (auto& RefreshBuff : ActiveDeBuffs)
			{
				// 해당 버프의 태그를 찾고
				if (RefreshBuff.BuffTag == Buff.BuffTag)
				{
					// 타이머 클리어 시키고
					GetWorld()->GetTimerManager().ClearTimer(RefreshBuff.TimerHandle);
					// 이미 있는 버프를 새로운 버프로 취급
					NewBuff = RefreshBuff;
					break;
				}
			}
		}
		// 없는 버프라면
		else
		{
			// 버프를 배열에 추가
			ActiveDeBuffs.Add(Buff);
			NewBuff = ActiveDeBuffs.Last();
		}
	}
	// 버프 시간이 갱신이 아닌 개별 시간이라면
	else if (Buff.BuffType == EBuffType::PERSTACK)
	{
		// 그냥 바로 추가해서 따로 타이머 돌리기
		ActiveDeBuffs.Add(Buff);
		NewBuff = ActiveDeBuffs.Last();
	}

	// 버프 스택수가 최대치를 넘어서면 최대치로 갱신
	if (*CurCount > Buff.MaxCount)
	{
		*CurCount = Buff.MaxCount;
	}

	FTimerDelegate Delegate;
	Delegate.BindLambda([this, BuffTag = NewBuff.BuffTag]()
		{
			RemoveBuff(BuffTag);
		});

	// 지속시간이 0이면 영구버프
	if (NewBuff.Duration > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			NewBuff.TimerHandle,
			Delegate,
			NewBuff.Duration,
			false
		);
	}
	else
	{
		// 영구버프는 나중에 구현
	}
}

void ABattleFieldPlayerState::CalculateBuff()
{
	FStatValues Add = {0.f,0.f,0.f,0.f,0.f };
	FStatValues Mul = { 1.f ,1.f ,1.f ,1.f ,1.f };

	for (const FBuffData& Buff : ActiveBuffs)
	{
		if (Buff.Contribute)
		{
			Buff.Contribute(Add, Mul, *TagCount.Find(Buff.BuffTag));
		}
	}

	for (const FBuffData& Buff : ActiveDeBuffs)
	{
		if (Buff.Contribute)
		{
			Buff.Contribute(Add, Mul, *TagCount.Find(Buff.BuffTag));
		}
	}

	TotalStats.Attack = (BaseStats.Attack + Add.Attack) * Mul.Attack;
	TotalStats.Defense = (BaseStats.Defense + Add.Defense) * Mul.Defense;
	TotalStats.AttackSpeed = (BaseStats.AttackSpeed + Add.AttackSpeed) * Mul.AttackSpeed;
	TotalStats.Speed = (BaseStats.Speed + Add.Speed) * Mul.Speed;
	TotalStats.LifeSteel = (BaseStats.LifeSteel + Add.LifeSteel) * Mul.LifeSteel;

	ActiveDeBuff();
}

void ABattleFieldPlayerState::ActiveDeBuff()
{
	// 서버에서 해야할지도
	APlayerCharacter* player = Cast<APlayerCharacter>(GetPawn());

	for (const FBuffData& Buff : ActiveDeBuffs)
	{
		if (Buff.BuffTag == FRedGameplayTags::Buff_CheonIJin_E_Stun)
		{
			player->GetCharacterMovement()->StopMovementImmediately();
			DisableInput(GetPlayerController());
			if (GetPlayerController())
			{
				GetPlayerController()->SetIgnoreMoveInput(true);
				GetPlayerController()->SetIgnoreLookInput(true);
				GetPlayerController()->bEnableClickEvents = false;
				GetPlayerController()->bEnableMouseOverEvents = false;
			}
		}
	}
}

void ABattleFieldPlayerState::RemoveDeBuff(const FBuffData& Buff)
{
	// 서버에서 해야할지도
	if (Buff.BuffTag == FRedGameplayTags::Buff_CheonIJin_E_Stun)
	{
		EnableInput(GetPlayerController());
		if (GetPlayerController())
		{
			GetPlayerController()->SetIgnoreMoveInput(false);
			GetPlayerController()->SetIgnoreLookInput(false);
			GetPlayerController()->bEnableClickEvents = true;
			GetPlayerController()->bEnableMouseOverEvents = true;
		}
	}
}

void ABattleFieldPlayerState::RemoveBuff(FGameplayTag BuffTag)
{
	FGameplayTag Tag;

	for (int32 i = 0; i < ActiveBuffs.Num(); ++i)
	{
		if (ActiveBuffs[i].BuffTag == BuffTag)
		{
			Tag = ActiveBuffs[i].BuffTag;
			ActiveBuffs.RemoveAt(i);
			break;
		}
	}

	for (int32 i = 0; i < ActiveDeBuffs.Num(); ++i)
	{
		if (ActiveDeBuffs[i].BuffTag == BuffTag)
		{
			Tag = ActiveDeBuffs[i].BuffTag;
			RemoveDeBuff(ActiveDeBuffs[i]);
			ActiveDeBuffs.RemoveAt(i);
			break;
		}
	}

	int32* Count = TagCount.Find(Tag);

	if (Count)
	{
		(*Count)--;

		if (*Count <= 0)
		{
			TagCount.Remove(Tag);
		}
	}

	CalculateBuff();
}

void ABattleFieldPlayerState::ClearBuff()
{
	for (auto debuff : ActiveDeBuffs)
	{
		RemoveDeBuff(debuff);
	}
	ActiveDeBuffs.Empty();
	ActiveBuffs.Empty();

	TagCount.Empty();

	CalculateBuff();
}