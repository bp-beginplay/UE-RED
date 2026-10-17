// Fill out your copyright notice in the Description page of Project Settings.


#include "HumanCharacter.h"

#include "NiagaraFunctionLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "RED/BattleField/BattleFieldPlayerState.h"

void AHumanCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//if (BattleFieldPlayerState && CharacterAction == ECharacterAction::Idle)
	//{
	//	float Health = BattleFieldPlayerState->Health;
	//	float MaxHealth = BattleFieldPlayerState->MaxHealth;
	//	if (Health < MaxHealth && GetVelocity().Size() == 0)
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("safe"));
	//		ReadyToHealTime -= DeltaTime;
	//		if (ReadyToHealTime <= 0)
	//		{
	//			NetMulti_PlayHealMontage();
	//			BattleFieldPlayerState->CalculateHealth(-10.f);
	//			ReadyToHealTime = 3.f;
	//		}
	//	}
	//	else
	//	{
	//		ReadyToHealTime = 3.f;
	//		UE_LOG(LogTemp, Warning, TEXT("not safe"));
	//	}
	//}
}

void AHumanCharacter::NetMulti_PlayHealMontage_Implementation()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HealMontage)
	{
		AnimInstance->Montage_Play(HealMontage);
	}
	if (HealEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			HealEffect,
			GetActorLocation(),
			GetActorRotation()
		);
	}
}
