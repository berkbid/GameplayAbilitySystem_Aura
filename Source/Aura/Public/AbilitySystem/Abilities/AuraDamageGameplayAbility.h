// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraAbilityTypes.h"
#include "AuraDamageGameplayAbility.generated.h"

class AActor;
class UObject;
class UGameplayEffect;
struct FGameplayTag;
struct FScalableFloat;

/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AURA_API void CauseDamage(AActor* TargetActor);
	
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Damage", meta=(GameplayTagFilter="Damage"))
	FGameplayTag DamageType;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Damage")
	FScalableFloat Damage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Debuff")
	float DebuffChance = 20.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Debuff")
	float DebuffDamage = 5.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Debuff")
	float DebuffFrequency = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Debuff")
	float DebuffDuration = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Death")
	float DeathImpulseMagnitude = 1000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Combat")
	float KnockbackChance = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Aura|Combat")
	float KnockbackMagnitude = 1000.f;
};
