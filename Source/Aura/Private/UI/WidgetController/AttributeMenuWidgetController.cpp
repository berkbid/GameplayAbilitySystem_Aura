// Copyright Berkeley Bidwell

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
	
	for (const TPair<FGameplayTag, FGameplayAttribute>& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value);
	}
}

void UAttributeMenuWidgetController::BindCallBacksToDependencies()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
	check(WidgetControllerParams.AbilitySystemComponent);

	// For each gameplay attribute, bind to it's value changing, then broadcast the attribute info using the tag and the new value
	for (const TPair<FGameplayTag, FGameplayAttribute>& Pair : AS->TagsToAttributes)
	{
		WidgetControllerParams.AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value);
			}
		);
	}
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	if (AttributeInfo && WidgetControllerParams.AttributeSet)
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
		Info.AttributeValue = Attribute.GetNumericValue(WidgetControllerParams.AttributeSet);
		AttributeInfoDelegate.Broadcast(Info);
	}
}
