// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraDamageGameplayAbility)

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	if (!ensure(DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Gameplay Ability missing damage effect class"));
	}

	UAbilitySystemComponent* SelfASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!SelfASC || !TargetASC)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot cause damage due to missing ASC of self or target!"));
		return;
	}
	
	// Make the spec handle
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);

	// Assign the set by caller magnitudes for all damage types in map
	for (const TTuple<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
	{
		// Will get the damage value at the ability/avatar's level
		const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
		
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}

	// Apply the damage to the target
	SelfASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);
}
