// Copyright Berkeley Bidwell

#pragma once

#include "AuraGameplayAbility.h"
#include "AuraPassiveGameplayAbility.generated.h"

class AActor;
class UObject;
struct FGameplayAbilitySpecHandle;
struct FGameplayAbilityActorInfo;
struct FGameplayAbilityActivationInfo;
struct FGameplayEventData;
struct FGameplayTag;


/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraPassiveGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
public:
	UAuraPassiveGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// UGameplayAbility interface
	AURA_API virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// ~UGameplayAbility interface
	
	// Callback from ASC telling this ability to deactivate
	AURA_API void ReceiveDeactivate(const FGameplayTag& AbilityTag);
};
