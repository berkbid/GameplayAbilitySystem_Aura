// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

class UObject;

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()


private:
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	
	
};
