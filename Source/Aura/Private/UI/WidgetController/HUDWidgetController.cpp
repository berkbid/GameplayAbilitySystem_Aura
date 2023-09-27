// Copyright Berkeley Bidwell

#include "UI/WidgetController/HUDWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UHUDWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
	
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
	OnManaChanged.Broadcast(AttributeSet->GetMana());
	OnMaxManaChanged.Broadcast(AttributeSet->GetMaxMana());
}

void UHUDWidgetController::BindCallBacksToDependencies()
{
	if (UAbilitySystemComponent* ASC = WidgetControllerParams.AbilitySystemComponent)
	{
		const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddUObject(this, &UHUDWidgetController::HealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddUObject(this, &UHUDWidgetController::MaxHealthChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddUObject(this, &UHUDWidgetController::ManaChanged);
		ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddUObject(this, &UHUDWidgetController::MaxManaChanged);
	}
}

void UHUDWidgetController::HealthChanged(const FOnAttributeChangeData& Data) const
{
	OnHealthChanged.Broadcast(Data.NewValue);
}

void UHUDWidgetController::MaxHealthChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHealthChanged.Broadcast(Data.NewValue);
}

void UHUDWidgetController::ManaChanged(const FOnAttributeChangeData& Data) const
{
	OnManaChanged.Broadcast(Data.NewValue);
}

void UHUDWidgetController::MaxManaChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxManaChanged.Broadcast(Data.NewValue);
}
