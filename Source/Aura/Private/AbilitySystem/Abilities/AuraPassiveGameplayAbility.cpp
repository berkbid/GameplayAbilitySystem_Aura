// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraPassiveGameplayAbility.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraPassiveGameplayAbility)

UAuraPassiveGameplayAbility::UAuraPassiveGameplayAbility(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

void UAuraPassiveGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(GetAbilitySystemComponentFromActorInfo()))
	{
		if (!AuraASC->OnDeactivatePassiveAbility.IsBoundToObject(this))
		{
			AuraASC->OnDeactivatePassiveAbility.AddUObject(this, &UAuraPassiveGameplayAbility::ReceiveDeactivate);
		}
	}
}

void UAuraPassiveGameplayAbility::ReceiveDeactivate(const FGameplayTag& AbilityTag)
{
	if (GetAssetTags().HasTagExact(AbilityTag))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}
