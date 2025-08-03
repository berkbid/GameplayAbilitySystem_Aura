// Copyright Berkeley Bidwell

#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxHealth.generated.h"

class UObject;
class UWorld;
struct FGameplayEffectSpec;
struct FGameplayEffectAttributeCaptureDefinition;

/**
 * 
 */
UCLASS(MinimalAPI)
class UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	AURA_API UMMC_MaxHealth();

	AURA_API virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
	AURA_API virtual FOnExternalGameplayModifierDependencyChange* GetExternalModifierDependencyMulticast(const FGameplayEffectSpec& Spec, UWorld* World) const override;
	
private:
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
