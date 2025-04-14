// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemGlobals.h"
#include "AuraAbilityTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAbilitySystemGlobals)

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	return new FAuraGameplayEffectContext();
}
