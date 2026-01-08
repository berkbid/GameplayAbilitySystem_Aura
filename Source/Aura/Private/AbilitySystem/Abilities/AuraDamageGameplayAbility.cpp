// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AuraLogChannels.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraDamageGameplayAbility)

UAuraDamageGameplayAbility::UAuraDamageGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Block activation if player is dead
	ActivationBlockedTags.AddTag(FAuraGameplayTags::Get().Player_Status_Dead);
	// Block activation if player is stunned
	ActivationBlockedTags.AddTag(FAuraGameplayTags::Get().Debuff_Stun);
}

void UAuraDamageGameplayAbility::CauseDamage(AActor* TargetActor)
{
	const FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults(TargetActor);
	
	if (!DamageEffectParams.SourceASC || !DamageEffectParams.TargetASC)
	{
		UE_LOG(LogAuraAbilitySystem, Log, TEXT("Cannot cause damage to [%s] due to missing ASC of self or target!"), *GetNameSafe( TargetActor));
		return;
	}
	
	UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
	
	/*
	if (!ensure(DamageEffectClass))
	{
		UE_LOG(LogTemp, Warning, TEXT("Gameplay Ability missing damage effect class"));
	}

	UAbilitySystemComponent* SelfASC = GetAbilitySystemComponentFromActorInfo();
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);

	if (!SelfASC || !TargetASC)
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot cause damage due to missing ASC of self or target!"));
		return FGameplayEffectSpecHandle();
	}
	
	// Make the spec handle
	FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
	
	// Will get the damage value at the ability/avatar's level
	const float ScaledDamage = Damage.GetValueAtLevel(GetAbilityLevel());
		
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, DamageType, ScaledDamage);
	
	// Apply the damage to the target
	SelfASC->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), TargetASC);

	// Returning this handle for now, not sure if necessary
	return DamageSpecHandle;
	*/
}

FDamageEffectParams UAuraDamageGameplayAbility::MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor) const
{
	FDamageEffectParams Params;
	Params.WorldContextObject = GetAvatarActorFromActorInfo();
	Params.DamageGameplayEffectClass = DamageEffectClass;
	Params.SourceASC = GetAbilitySystemComponentFromActorInfo();
	Params.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	Params.BaseDamage = Damage.GetValueAtLevel(GetAbilityLevel());
	Params.AbilityLevel = GetAbilityLevel();
	Params.DamageType = DamageType;
	Params.DebuffChance = DebuffChance;
	Params.DebuffDamage = DebuffDamage;
	Params.DebuffFrequency = DebuffFrequency;
	Params.DebuffDuration = DebuffDuration;
	Params.DeathImpulseMagnitude = DeathImpulseMagnitude;
	Params.KnockbackMagnitude = KnockbackMagnitude;
	Params.KnockbackChance = KnockbackChance;
	
	if (IsValid(TargetActor))
	{
		FRotator Rotation = (TargetActor->GetActorLocation() - GetAvatarActorFromActorInfo()->GetActorLocation()).Rotation();
		// This will utilize the knockback chance to determine knockback value
		Params.UpdateImpulsesWithRotation(Rotation);
	}
	
	return Params;
}

float UAuraDamageGameplayAbility::GetDamageAtLevel() const
{
	return Damage.GetValueAtLevel(GetAbilityLevel());
}
