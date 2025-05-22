// Copyright Berkeley Bidwell

#include "Input/AuraInputConfig.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraInputConfig)

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	if (const FAuraInputAction* FoundInputAction = AbilityInputActions.FindByPredicate([InputTag](const FAuraInputAction& AuraInputAction)
		{ return AuraInputAction.InputAction && AuraInputAction.InputTag == InputTag; }))
	{
		return FoundInputAction->InputAction;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find input action for input tag [%s] on AuraInputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}
	
	return  nullptr;
}
