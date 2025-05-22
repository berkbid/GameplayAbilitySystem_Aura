// Copyright Berkeley Bidwell

#pragma once

#include "EnhancedInputComponent.h"
#include "AuraInputConfig.h"
#include "AuraInputComponent.generated.h"

class UObject;

/**
* UAuraInputComponent
 *
 *	Component used to manage input mappings and bindings using an input config data asset.
 */
UCLASS(Config = Input)
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
	
};

template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig && Object);
	
	for (const FAuraInputAction& AuraInputAction : InputConfig->AbilityInputActions)
	{
		if (AuraInputAction.InputAction && AuraInputAction.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(AuraInputAction.InputAction, ETriggerEvent::Started, Object, PressedFunc, AuraInputAction.InputTag);
			}
			if (ReleasedFunc)
			{
				BindAction(AuraInputAction.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, AuraInputAction.InputTag);
			}
			if (HeldFunc)
			{
				BindAction(AuraInputAction.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, AuraInputAction.InputTag);
			}
		}
	}
}
