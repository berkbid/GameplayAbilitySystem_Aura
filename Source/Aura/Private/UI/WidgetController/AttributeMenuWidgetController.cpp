// Copyright Berkeley Bidwell

#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttributeMenuWidgetController)

void UAttributeMenuWidgetController::BindCallBacksToDependencies()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
	check(WidgetControllerParams.AbilitySystemComponent);

	// For each gameplay attribute, bind to its value changing, then broadcast the attribute info using the tag and the new value
	for (const TPair<FGameplayTag, FGameplayAttribute>& Pair : AS->TagsToAttributes)
	{
		WidgetControllerParams.AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value).AddLambda(
			[this, Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value);
			}
		);
	}

	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(WidgetControllerParams.PlayerState);
	
	// Attribute points changed
	AuraPS->OnAttributeUpdate.AddLambda([this](int32 NewAttributePoints)
	{
		OnAttributePointsChanged.Broadcast(NewAttributePoints);
	});

	// Spell points changed
	AuraPS->OnSpellPointUpdate.AddLambda([this](int32 NewSpellPoints)
	{
		OnSpellPointsChanged.Broadcast(NewSpellPoints);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
	
	for (const TPair<FGameplayTag, FGameplayAttribute>& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value);
	}
	
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(WidgetControllerParams.PlayerState);
	if (OnAttributePointsChanged.IsBound()) { OnAttributePointsChanged.Broadcast(AuraPS->GetAttributePoints()); }
	if (OnSpellPointsChanged.IsBound()) { OnSpellPointsChanged.Broadcast(AuraPS->GetSpellPoints()); }
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

void UAttributeMenuWidgetController::IncrementAttribute(const FGameplayTag& AttributeTag, int32 IncrementAmount)
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	ASC->AddOrRefundAttribute(AttributeTag, IncrementAmount);
}
