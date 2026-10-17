// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SkillComponent.generated.h"

UENUM(BlueprintType)
enum class ESkillState : uint8
{
	Idle UMETA(DisplayName = "Idle"),
	SetTarget UMETA(DisplayName = "SetTarget"),
	CanActive UMETA(DisplayName = "CanActive"),
	Active UMETA(DisplayName = "Active"),
	Cooldown UMETA(DisplayName = "Cooldown"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RED_API USkillComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	virtual void SkillSetTarget();
	virtual void SkillStart();
	virtual void SkillStart(AActor* _Actor);
	virtual void SkillStart(FVector _TargetLocation);
	UFUNCTION(Server, Reliable)
	void Server_SetTargetActor(AActor* _TargetActor);
	UFUNCTION(Server, Reliable)
	void Server_SetbAutoRun(bool _bAutoRun);


protected:
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;
	
	virtual void SkillActive();
	virtual void SkillCooldown();
	virtual void SkillCanActive();
	bool bPlayAnimMontage = false;
	bool bStartAttack = false;
	UFUNCTION(Client, Reliable)
	void Client_SkillCountTime();
	UFUNCTION(Client, Reliable)
	void Client_ChangeSkillState(ESkillState _SkillState, float _Time);
	UFUNCTION(Client, Reliable)
	void Client_UpdateCoolTimeTextBlock(float _CoolTime);

	virtual void AutoRun();
	virtual void PlayAnimMontage(UAnimMontage* _Montage);

public:
	UFUNCTION(Server, Reliable)
	void Server_ChangeSkillState(ESkillState _SkillState);

	UFUNCTION()
	ESkillState GetSkillState() const { return SkillState; }
protected:
	UFUNCTION(Server, Reliable)
	void Server_UpdateSkillCountTextBlock();
	UFUNCTION(Client, Reliable)
	void Client_SetSkillIconOpacity(bool _CanActive);

	UFUNCTION(Server, Reliable, BlueprintCallable)
	void Server_Fire();
	virtual void Server_Fire_Implementation();

	UFUNCTION(BlueprintCallable)
	virtual void EndAnimMontage();
	UFUNCTION(Server, Reliable)
	void Server_SetTargetLocation(FVector _TargetLocation);
private:
	UFUNCTION()
	virtual void OnRep_SkillState();


	UFUNCTION(Server, Reliable)
	void Server_AddSkillCount();
	void UpdateCoolTimeTextBlock();

	UFUNCTION()
	void OnRep_SkillCount();
	void UpdateSkillCountTextBlock();


public:
	UPROPERTY()
	class APlayerCharacter* PlayerCharacter;
	UPROPERTY()
	class USkillIcon* SkillIcon;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;
	UPROPERTY(EditAnywhere)
	float ArrangeRadius = 0.f;
	UPROPERTY(Replicated)
	bool bAutoRun = false;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Replicated)
	AActor* TargetActor;

	int GetSkillCount() const { return SkillCount; }

protected:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, ReplicatedUsing = OnRep_SkillState)
	ESkillState SkillState = ESkillState::CanActive;
	UPROPERTY(EditAnywhere)
	float ActiveTime = 0.05f;
	UPROPERTY(EditAnywhere)
	float CoolTime;
	UPROPERTY(ReplicatedUsing = OnRep_SkillCount, EditAnywhere)
	int SkillCount = 0;
	UPROPERTY(EditAnywhere)
	int InitSkillCount = 0;
	UPROPERTY(EditAnywhere)
	class UAnimMontage* SkillActiveMontage;


	UPROPERTY(Replicated)
	FVector TargetLocation;

private:
	float CountDown = 0.f;
	FTimerHandle CoolTimeTimerHandle;
	FTimerHandle SkillCountTimerHandle;
};
