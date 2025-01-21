// Copyright Berkeley Bidwell

#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "MMC_MaxHealth.generated.h"

class UObject;
struct FGameplayEffectSpec;
struct FGameplayEffectAttributeCaptureDefinition;

/**
 * 
 */
UCLASS()
class AURA_API UMMC_MaxHealth : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()

public:
	UMMC_MaxHealth();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:
	FGameplayEffectAttributeCaptureDefinition VigorDef;
};
