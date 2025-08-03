// Copyright Berkeley Bidwell

#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxMana.generated.h"

class UObject;
class UWorld;
struct FGameplayEffectSpec;
struct FGameplayEffectAttributeCaptureDefinition;

/**
 * 
 */
UCLASS(MinimalAPI)
class UMMC_MaxMana : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	AURA_API UMMC_MaxMana();

	AURA_API virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	AURA_API virtual FOnExternalGameplayModifierDependencyChange* GetExternalModifierDependencyMulticast(const FGameplayEffectSpec& Spec, UWorld* World) const override;
	
private:
	FGameplayEffectAttributeCaptureDefinition IntelligenceDef;
};
