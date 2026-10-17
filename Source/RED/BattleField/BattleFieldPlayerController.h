// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "RED/Info/CharacterListRow.h"
#include "RED/Info/EnumInfo.h"
#include "../Interface/ActorInterface.h"
#include "BattleFieldPlayerController.generated.h"

class UNiagaraSystem;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class RED_API ABattleFieldPlayerController : public APlayerController, public IActorInterface
{
	GENERATED_BODY()

public:
	ABattleFieldPlayerController();
	virtual void PlayerTick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	void InitPlayer();
	UFUNCTION(Client, Reliable)
	void UpdateTeamScoreTextBlock(int _RedTeamScore, int _BlueTeamScore);
	UFUNCTION(Client, Reliable)
	void AnnounceWinner(ETeam _Team);
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	virtual void SetupInputComponent() override;

	void UpdateHealthProgressBar(float MaxHealth, float Health, float ShieldAmount);
	void UpdateManaProgressBar(float MaxMana, float Mana);
	void InitCharacterImage(FCharacterListRow* CharacterListRow);

	void CursorTrace();
	void AutoRun();
	void CreateNavPath(FVector Destination);

	void EdgeScroll();
	void Right(float AxisValue);
	void Forward(float AxisValue);
	void Zoom(const FInputActionValue& InputActionValue);
	void FixCamera();

	void RMB_Pressed();
	void RMB_Released();
	void RMB_Held();
	void LMB_Pressed();
	void LMB_Released();
	void A_Pressed();
	void Q_Pressed();
	void W_Pressed();
	void E_Pressed();
	void R_Pressed();
	void M_Pressed();
	void SpaceBar_Pressed();
	void SpaceBar_Released();

	void Map_LMB_Pressed(FVector2D Location);
	void Map_LMB_Released(FVector2D Location);
	void Map_RMB_Pressed(FVector2D Location);
	void CamLocationFix();

	UFUNCTION(BlueprintCallable)
	void SetControlPointValue(int32 _Value);
	UFUNCTION(Server, Reliable)
	void Server_SetMinionMovePoint(int32 _Value);

	UPROPERTY()
	class ABattleFieldHUD* BattleFieldHUD;
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> CharacterDataTable;

	UPROPERTY()
	AActor* TargetActor;

	UPROPERTY(EditAnywhere)
	float ArrivalRange = 70.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputMappingContext> IMC_Default;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_RMB;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_Scroll;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_Y;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_A;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_LMB;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_Q;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_W;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_E;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_R;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_M;
	UPROPERTY(EditAnywhere)
	TObjectPtr<UInputAction> IA_Space;

	bool Space_Pressed = false;;
	bool Map_LMB_Clicked = false;

	FHitResult CursorHit;
	TArray<FVector> PathPointArray;
	FVector CachedDestination;
	float ShortPressThreshold = 0.5f;
	int PathPointArrayIndex = 1;
	bool bAutoRunning = false;

	UPROPERTY(EditAnywhere)
	float EdgeScrollSpeed = 10.0f;
	UPROPERTY(EditAnywhere)
	float CameraMoveSpeed = 20.0f;
	FVector TargetLocation;
	bool IsFixCamera = true;

	UPROPERTY(EditAnywhere)
	float ZoomSpeed = 100.0f;
	UPROPERTY(EditAnywhere)
	float MinZoom = 100.0f;
	UPROPERTY(EditAnywhere)
	float MaxZoom = 4000.0f;
	float TargetZoom;

	UPROPERTY()
	AActor* LastActor;
	UPROPERTY()
	AActor* ThisActor;

	UPROPERTY(EditAnywhere, Replicated)
	ETeam Team = ETeam::ET_NoTeam;

	UPROPERTY(Replicated)
	bool ControllerReady = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widget")
	class USelectControlZoneUI* SelectControlPointUI;

	UFUNCTION()
	void ChangeControlPointUIButton();

	UFUNCTION(BlueprintCallable)
	void SetTeam(ETeam _Team) { Team = _Team; }

	virtual int32 GetTeam_Implementation() const override
	{
		return static_cast<int32>(Team);
	}

	virtual ETeam GetETeam_Implementation() const override
	{
		return Team;
	}

	virtual void SetControllerReady(bool IsReady) override;

	UFUNCTION(Server,Reliable)
	void Server_ControllerReady();

	void FadeOutImage(bool _IsDied);

	UFUNCTION(BlueprintCallable)
	void RequestRespawn(AController* ElimmedController, int32 RespawnTargetIdx);

	UFUNCTION(Server, Reliable)
	void Server_SetFOWReplicationTeam();

	UFUNCTION(Server, Reliable)
	void Server_TeamSetting(APlayerCharacter* _Player);

	UFUNCTION(Client, Reliable)
	void Client_RemoveLoading();
};
