// Copyright Berkeley Bidwell

#pragma once

#include "GameplayModMagnitudeCalculation.h"
#include "MMC_AbilityCost.generated.h"

class UObject;
struct FGameplayEffectSpec;

/**
 * 
 */
UCLASS(MinimalAPI)
class UMMC_AbilityCost : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	AURA_API UMMC_AbilityCost();

	AURA_API virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;
};
