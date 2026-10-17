// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "RED/Interface/ActorInterface.h"
#include "RED/Info/EnumInfo.h"
#include "GameplayTagContainer.h"
#include "Entity/FOW_VisibilityEntity_Interface.h"

#include "BaseCharacter.generated.h"

UCLASS()
class RED_API ABaseCharacter : public ACharacter, public IActorInterface
{
	GENERATED_BODY()

public:
	ABaseCharacter();
	
	UFUNCTION()
	virtual void OnRep_IsDied();

	UFUNCTION()
	virtual void UpdateCollision();

	UFUNCTION()
	void Update_RespawnCollision();

	void Attack(FRotator _TargetRotation);
	void StopMontage();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void PostInitializeComponents() override;

	UFUNCTION()
	virtual void OnRep_Ground();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class UStaticMeshComponent* MiniMapIcon;
private:
	UFUNCTION(Server, Reliable)
	void Server_Attack(FRotator _TargetRotation);
	UFUNCTION(NetMulticast, Reliable)
	void NetMulti_Attack(FRotator _TargetRotation);
	void PlayAttackMontage(FRotator _TargetRotation);
	UFUNCTION(BlueprintCallable)
	void EndAttackMontage();

	void SetDrawerTeam_OnDied();
public:
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ECharacterAction CharacterAction = ECharacterAction::Idle;
	UPROPERTY(ReplicatedUsing = OnRep_Ground, EditAnywhere, BlueprintReadWrite)
	EGround Ground = EGround::Ground;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tags")
	FGameplayTagContainer GamePlayTags;
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	ETeam Team = ETeam::ET_NoTeam;
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_IsDied)
	bool IsDied = false;
protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FOW")
	class UFOW_Drawer_FOV* FOWDrawer_FOV;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "FOW")
	class UFOW_VisibilityE_Component* FOW_VisibilityE;

	UPROPERTY(VisibleAnywhere, Category = "UI")
	class UWidgetComponent* HPWidgetComp;
private:
	UPROPERTY(EditAnywhere)
	UAnimMontage* AttackMontage;

public:
	virtual void HighlightActor_Implementation() override;
	virtual void UnHighlightActor_Implementation() override;

	virtual int32 GetTeam_Implementation() const override
	{
		return static_cast<int32>(Team);
	}
	virtual ETeam GetETeam_Implementation() const override
	{
		return Team;
	}
	virtual bool GetIsDied_Implementation() const override
	{
		return IsDied;
	}

	virtual int32 GetVisibility_Implementation() const override;

	UFUNCTION(Server, Reliable, BlueprintCallable)
	virtual void Server_SetTeam(ETeam _Team);

	UFUNCTION()
	bool IsEnemy(AActor* _OtherActor) const;

	UFUNCTION(NetMulticast, Reliable)
	void Mul_SetHPBarColor(ETeam _Team);
};

