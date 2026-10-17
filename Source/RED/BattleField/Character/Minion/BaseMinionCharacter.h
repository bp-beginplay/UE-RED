// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../BaseCharacter.h"
#include "Entity/FOW_VisibilityEntity_Interface.h"
#include "BaseMinionCharacter.generated.h"

UCLASS()
class RED_API ABaseMinionCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseMinionCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// 감지범위용 실린더 만들기
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Component")
	UStaticMeshComponent* DetectionRange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Target")
	int32 TargetControlPointValue = 0;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IsAttacking)
	bool IsAttacking = false;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MinionStats)
	EMinionType MinionType = EMinionType::NONE;

	UPROPERTY(EditAnywhere, Category = "Minion")
	TSoftObjectPtr<USkeletalMesh> RedTeamMeleeMinionMesh;

	UPROPERTY(EditAnywhere, Category = "Minion")
	TSoftObjectPtr<USkeletalMesh> RedTeamRangeMinionMesh;

	UPROPERTY(EditAnywhere, Category = "Minion")
	TSoftObjectPtr<USkeletalMesh> BlueTeamMeleeMinionMesh;

	UPROPERTY(EditAnywhere, Category = "Minion")
	TSoftObjectPtr<USkeletalMesh> BlueTeamRangeMinionMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<class UHealthComponent> HealthComponent;

	void UpdateMesh();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void SetTargetControlPointValue(int32 _TargetControlPointValue);

	UFUNCTION()
	int32 GetTargetControlPointValue() const { return TargetControlPointValue; }

	virtual int32 GetTeam_Implementation() const override
	{
		return static_cast<int32>(Team);
	}

	UFUNCTION(Server, Reliable)
	void Server_SetFOWDrawerTeam(int32 _Team) const;

	UFUNCTION(BlueprintCallable)
	void SetIsAttacking(bool _IsAttacking);

	UFUNCTION()
	void OnRep_IsAttacking();

	UFUNCTION(BlueprintCallable)
	bool GetIsAttacking() { return IsAttacking; }

	void SetIsMelee(EMinionType _MinionType) { MinionType = _MinionType; }

	UFUNCTION()
	void OnRep_MinionStats();

	UFUNCTION()
	void OnRep_HP();

	void SetHP(float _HP);
	float GetHP();

	void SetMaxHP(float _MaxHP);
	float GetMaxHP();

	UFUNCTION(NetMulticast, Reliable)
	void Mul_NotifyHPChanged(float _CurHP, float _Shield, float _MaxHP);
protected:
	UFUNCTION(BlueprintCallable)
	void OnDetectionRangeBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION(BlueprintCallable)
	void OnDetectionRangeEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	void NotifyBeginOverlap(AActor* _Actor);

	void NotifyEndOverlap(AActor* _Actor);

	void NotifyOnDestroyed(AActor* _Actor);

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

	FTimerHandle GoDownHandle;

	void GoDown();

	UFUNCTION()
	void Died();

	virtual void OnRep_IsDied() override;

	virtual void UpdateCollision() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

};
