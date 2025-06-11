// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraGameplayAbility_Summon.generated.h"

class UObject;
class AAuraEnemy;

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility_Summon : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAuraGameplayAbility_Summon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetSpawnLocations();
	
	UFUNCTION(BlueprintPure, Category="Summon")
	TSubclassOf<AAuraEnemy> GetRandomMinionClass();
	
	UFUNCTION(BlueprintPure, Category="Summon")
	AAuraEnemy* GetRandomMinionClassCDO();
	
	// Clamp because value set with SafeNetSerialize_WithNetSerialize in effect context for spawn locations
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Summon", meta = (ClampMin = "0", ClampMax = "31"))
	int32 NumMinions = 5;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Summon")
	TArray<TSubclassOf<AAuraEnemy>> MinionClasses;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Summon")
	float MinSpawnDistance = 50.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Summon")
	float MaxSpawnDistance = 250.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Summon")
	float SpawnSpread = 90.f;
};
