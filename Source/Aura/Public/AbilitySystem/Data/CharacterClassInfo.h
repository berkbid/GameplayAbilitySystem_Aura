// Copyright Berkeley Bidwell

#pragma once

#include "Engine/DataAsset.h"
#include "ScalableFloat.h"
#include "CharacterClassInfo.generated.h"

class UObject;
class UGameplayAbility;
class UGameplayEffect;
class UCurveTable;

UENUM(BlueprintType)
enum class ECharacterClass : uint8
{
	Warrior,
	Ranger,
	Elementalist
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	/** Gameplay effect to apply primary attributes */
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes;

	/** Common abilities specific to this class to grant by default */
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> StartupAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Class Defaults")
	FScalableFloat XpReward = FScalableFloat();
};

/**
 * 
 */
UCLASS()
class UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Common Class Defaults")
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass) const;
	
public:
	/** Gameplay effect to apply secondary attributes */
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;
	
	/** Gameplay effect to apply vital attributes */
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;
	
	UPROPERTY(EditDefaultsOnly, Category = "Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;
	
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults|Damage")
	TObjectPtr<UCurveTable> DamageCalculationCoefficients;
};
