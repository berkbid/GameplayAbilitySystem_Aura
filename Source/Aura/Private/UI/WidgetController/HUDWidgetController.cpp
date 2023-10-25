// Copyright Berkeley Bidwell

#include "UI/WidgetController/HUDWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UHUDWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);

	UE_LOG(LogTemp, Warning, TEXT("Broadcasting initial HUD values"));
	
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AttributeSet->GetMaxMana());
}

void UHUDWidgetController::BindCallBacksToDependencies()
{
	if (UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent))
	{
		const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
		
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			});
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			});
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnManaChanged.Broadcast(Data.NewValue);
			});
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxManaChanged.Broadcast(Data.NewValue);
			});
		ASC->EffectAssetTags.AddLambda(
			[this](const FGameplayTagContainer& AssetTags)
			{
				const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));

				// Broadcast all message tags
				for (const FGameplayTag& Tag : AssetTags)
				{
					if (Tag.MatchesTag(MessageTag))
					{
						UE_LOG(LogTemp, Warning, TEXT("Effect has message tag: %s"), *Tag.ToString());
						if (const FUIWidgetRow* TagRow = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag))
						{
							MessageWidgetRow.Broadcast(*TagRow);
						}
					}
				}
			}
		);
	}
}