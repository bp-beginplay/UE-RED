// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

UCLASS()
class RED_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	virtual void A_Skill();
	virtual void Q_Skill();
	virtual void W_Skill();
	virtual void E_Skill();
	virtual void R_Skill();
	virtual void LMB_Pressed();
	virtual void RMB_Pressed();
	virtual void LMB_Released(FVector _TargetPosition, AActor* _Actor);
	virtual void RMB_Released();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UAttackComponent* AttackComponent;
protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

	UFUNCTION()
	virtual void Tick(float DeltaTime) override;

	virtual void OnRep_PlayerState() override;

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CanRespawn)
	bool CanRespawn = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* SkillRange1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* SkillRange2;
private:
	UFUNCTION()
	virtual void BeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void EndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class UCameraComponent> Camera;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere)
	class USphereComponent* AttackRangeCollision;
	UPROPERTY(VisibleAnywhere)
	TArray<AActor*> TargetActorArray;

	void UpdateBar(float _CurHP, float _ShieldAmount, float _MaxHP);

	void UpdateBar1();

	void SetCanRespawn(bool Respawn) { CanRespawn = Respawn; }
	bool GetCanRespawn() const { return CanRespawn; }

	UFUNCTION(Server, Reliable)
	void Server_Respawn(AController* ElimmedController, int32 RespawnTargetIdx);

	UFUNCTION(Server, Reliable)
	void Server_Update_RespawnCollision();

	void LifeSteal(float TotalDamage);
private:
	// 위젯 업데이트 시점 잡기용
	bool bWidgetReady = false;
	bool bPlayerStateReady = false;

	float SafeTime = 5.f;

protected:
	UPROPERTY()
	class ABattleFieldPlayerState* BattleFieldPlayerState;
	
	UFUNCTION()
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	void SetCanRespawnTrue();

	UFUNCTION()
	void OnRep_CanRespawn();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MiniMap")
	class USceneCaptureComponent2D* MiniMapCapture;

	UPROPERTY(EditAnywhere)
	UTextureRenderTarget2D* MinimapRT;

	UFUNCTION()
	void InitMinimapUI();
};
