// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraGameplayAbility_Melee.generated.h"

class UObject;
struct FFrame;
//struct FGameplayAbilityActorInfo;
//struct FGameplayTagContainer;

/**
 * UAuraGameplayAbility_Melee
 *
 *	Gameplay ability used for character jumping.
 */
UCLASS(Abstract)
class UAuraGameplayAbility_Melee : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAuraGameplayAbility_Melee(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
};
