// Copyright Berkeley Bidwell

#include "AbilitySystem/ModMagCalc/MMC_AbilityCost.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MMC_AbilityCost)

UMMC_AbilityCost::UMMC_AbilityCost()
{

}

float UMMC_AbilityCost::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const UAuraGameplayAbility* Ability = Cast<UAuraGameplayAbility>(Spec.GetContext().GetAbilityInstance_NotReplicated());

	if (!Ability)
	{
		return 0.0f;
	}

	return Ability->Cost.GetValueAtLevel(Ability->GetAbilityLevel());
}
