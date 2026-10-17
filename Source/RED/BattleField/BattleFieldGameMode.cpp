// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleFieldGameMode.h"

#include "BattleFieldGameState.h"
#include "Kismet/GameplayStatics.h"

#include "BattleFieldPlayerState.h"
#include "BattleFieldPlayerController.h"
#include "Actor/ControlPoint/ControlPointBaseActor.h"
#include "Actor/BaseCamp.h"
#include "Character/PlayerCharacter.h"
#include "Network/FOW_NetworkSettings.h"
#include "Network/Replication/FOW_FogStateReplication_Client.h"
#include "FOW_Handler.h"
#include "GameFramework/GameStateBase.h"
#include "Network/FOW_FogStateReplication.h"
#include "Network/Replication/FOW_FogStateReplication_Server.h"
#include "RED/RED.h"

#include "RED/Info/CharacterListRow.h"
#include "RED/Info/EnumInfo.h"
#include "RED/Info/RedGamePlayTags.h"
#include "RED/Interface/ActorInterface.h"


ABattleFieldGameMode::ABattleFieldGameMode()
{
	bUseSeamlessTravel = true;
}

void ABattleFieldGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController, int32 RespawnTargetIdx)
{
	AControlPointBaseActor* TempControlPoint = nullptr;

	for (AControlPointBaseActor* controlpoint : ControlPointList)
	{
		if (controlpoint->GetControlPointValue() == RespawnTargetIdx)
		{
			TempControlPoint = controlpoint;
			break;
		}
	}

	if (TempControlPoint)
	{
		if (IActorInterface::Execute_GetETeam(TempControlPoint) != IActorInterface::Execute_GetETeam(ElimmedController))
		{
			return;
		}

		ElimmedCharacter->SetActorLocation(TempControlPoint->GetActorLocation() + FVector{0,0,100});
		ElimmedCharacter->SetActorRotation(TempControlPoint->GetActorRotation());

		ABattleFieldPlayerController* PC = Cast<ABattleFieldPlayerController>(ElimmedController);
		PC->InitPlayer();
		Cast<APlayerCharacter>(ElimmedCharacter)->SetCanRespawn(false);
	}
}

void ABattleFieldGameMode::UpdateTeamScore(ETeam _Team, int _Score)
{
	switch (_Team)
	{
	case ETeam::ET_RedTeam:
		RedTeamScore += _Score;
		break;
	case ETeam::ET_BlueTeam:
		BlueTeamScore += _Score;
		break;
	case ETeam::ET_NoTeam:
		break;
	default:
		break;
	}

	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		ABattleFieldPlayerController* BattleFieldPlayerController = Cast<ABattleFieldPlayerController>(*It);
		BattleFieldPlayerController->UpdateTeamScoreTextBlock(RedTeamScore, BlueTeamScore);
	}
	if (RedTeamScore == 3 || BlueTeamScore == 3)
	{
		ETeam Team = ETeam::ET_NoTeam;
		if (RedTeamScore == 3)
		{
			Team = ETeam::ET_RedTeam;
		}
		if (BlueTeamScore == 3)
		{
			Team = ETeam::ET_BlueTeam;
		}
		for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		{
			ABattleFieldPlayerController* BattleFieldPlayerController = Cast<ABattleFieldPlayerController>(*It);
			BattleFieldPlayerController->AnnounceWinner(Team);
		}

		FTimerHandle TimerHandle;
		GetWorldTimerManager().SetTimer(
			TimerHandle,
			this,
			&ABattleFieldGameMode::EndGame,
			5.0f,
			false
		);
	}
}

void ABattleFieldGameMode::EndGame()
{
	bUseSeamlessTravel = false;
	UWorld* World = GetWorld();
	if (World)
	{
		World->ServerTravel(FString("/Game/Maps/Startup"));
	}
}

void ABattleFieldGameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
}

UClass* ABattleFieldGameMode::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	FName SelectCharacterName = InController->GetPlayerState<ABattleFieldPlayerState>()->GetPickCharacter();
	if (!SelectCharacterName.IsNone())
	{
		FCharacterListRow* CharacterListRow = CharacterDataTable->FindRow<FCharacterListRow>(SelectCharacterName, TEXT(""));
		return CharacterListRow->CharacterClass;
	}
	return nullptr;
}

AActor* ABattleFieldGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	Super::ChoosePlayerStart_Implementation(Player);
	TArray<AActor*> PlayerStarts;
	UGameplayStatics::GetAllActorsOfClass(this, ABaseCamp::StaticClass(), PlayerStarts);
	ABattleFieldPlayerState* PS = Player->GetPlayerState<ABattleFieldPlayerState>();
	for (auto& iter : PlayerStarts)
	{
		if (IActorInterface::Execute_GetETeam(iter) == IActorInterface::Execute_GetETeam(PS))
		{
			return iter;
		}
	}
	return nullptr;
}

void ABattleFieldGameMode::BeginPlay()
{
	Super::BeginPlay();

	FRedGameplayTags::InitializeTags();

	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AControlPointBaseActor::StaticClass(),
		FoundActors
	);

	for (AActor* Actor : FoundActors)
	{
		if (AControlPointBaseActor* ControlPoint = Cast<AControlPointBaseActor>(Actor))
		{
			check(ControlPoint);
			ControlPointList.Add(ControlPoint);
		}
	}

	ABattleFieldGameState* BGS = Cast<ABattleFieldGameState>(GameState);
	if (BGS)
	{
		BGS->InitGameState();
	}

	ExpectedPlayerCount = GetNumPlayers();

	UE_LOG(LogTemp, Error, TEXT("ExpectedPlayerCount : %d"), ExpectedPlayerCount);
}

void ABattleFieldGameMode::HandleSeamlessTravelPlayer(AController*& C)
{
	//Super::HandleSeamlessTravelPlayer(C) 전에는 로비컨트롤러, 후에 배틀필트 컨트롤러

	Super::HandleSeamlessTravelPlayer(C);
}

void ABattleFieldGameMode::NotifyPlayerReady()
{
	ReadyPlayerCount++;
	UE_LOG(LogTemp, Warning, TEXT("NotifyPlayerReady"));

	if (ReadyPlayerCount >= ExpectedPlayerCount)
	{
		AllPlayersReady();
	}
}

void ABattleFieldGameMode::AllPlayersReady()
{
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		APlayerController* PC = It->Get();
		if (!PC)
		{
			continue;
		}

		ABattleFieldPlayerController* BFPC = Cast<ABattleFieldPlayerController>(PC);
		if (!BFPC)
		{
			continue;
		}

		FTimerHandle LoadingHandle;
		FTimerDelegate LoadingDelegate;

		LoadingDelegate.BindLambda([this, BFPC]()
			{
				BFPC->Client_RemoveLoading();
			});

		GetWorld()->GetTimerManager().SetTimer(
			LoadingHandle,
			LoadingDelegate,
			5.f,
			false
			);
	}

	UE_LOG(LogTemp, Warning, TEXT("Game Start"));

	FTimerHandle GameStartHandle;
	FTimerDelegate GameStartDelegate;

	GameStartDelegate.BindLambda([this]()
		{
			GameStart = true;
		});

	GetWorld()->GetTimerManager().SetTimer(
		GameStartHandle,
		GameStartDelegate,
		5.f,
		false
	);

	FTimerHandle GameTimeHandle;
	GetWorld()->GetTimerManager().SetTimer(
		GameTimeHandle,
		this,
		&ABattleFieldGameMode::IncreaseGameTime,
		1.f,
		true,
		5.f
	);

}

void ABattleFieldGameMode::IncreaseGameTime()
{
	GameTime += 1.f;
}