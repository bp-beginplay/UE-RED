#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "CharacterListRow.generated.h"

USTRUCT(BlueprintType)
struct FCharacterListRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<class APlayerCharacter> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitMaxHealth;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitMaxMana;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UTexture2D* CharacterImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* PassiveImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* QSkillImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* WSkillImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* ESkillImage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* RSkillImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText PassiveDescription = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText QSkillDescription = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText WSkillDescription = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText ESkillDescription = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText RSkillDescription = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitAttack;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitDefense;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitAttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InitLifeSteel;
};