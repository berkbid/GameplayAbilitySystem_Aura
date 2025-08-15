// Copyright Berkeley Bidwell

#include "UI/WidgetController/AuraWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "System/AuraGameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraWidgetController)

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& InWidgetControllerParams)
{
	// This happens one time when the controller is created, before it it set on any widgets
	
	WidgetControllerParams = InWidgetControllerParams;
	BindCallBacksToDependencies(); // Set up binds after we get references
}

void UAuraWidgetController::BroadcastAbilityInfo() const
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	check(ASC);
	
	if (!ASC->bStartupAbilitiesGiven) { return; }
	
	// Create delegate and bind lambda to it
	FForEachAbility BroadcastDelegate;
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		const UAbilityInfo* AbilityInfo = GetAbilityInfo();
		if (!AbilityInfo) { return; }
		
		const FGameplayTag AbilityTag = UAuraAbilitySystemComponent::GetAbilityTagFromSpec(AbilitySpec);

		// Ability tag must be valid (ex. GA_ListenForEvents)
		if (!AbilityTag.IsValid()) { return; }
		
		FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfo(AbilityTag);
		Info.InputTag = UAuraAbilitySystemComponent::GetInputTagFromSpec(AbilitySpec);
		Info.StatusTag = UAuraAbilitySystemComponent::GetStatusTagFromSpec(AbilitySpec);
		
		AbilityInfoDelegate.Broadcast(Info);
	});

	// For each activatable ability, will execute lambda above with their ability spec
	ASC->ForEachAbility(BroadcastDelegate);
}

UAbilityInfo* UAuraWidgetController::GetAbilityInfo() const
{
	const UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetWorld()->GetGameInstance());
	return AuraGameInstance ? AuraGameInstance->AbilityInfo : nullptr;
}
