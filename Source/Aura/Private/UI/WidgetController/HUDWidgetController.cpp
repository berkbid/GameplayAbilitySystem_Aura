// Copyright Berkeley Bidwell

#include "UI/WidgetController/HUDWidgetController.h"
#include "AbilitySystem/AuraAttributeSet.h"

void UHUDWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
	
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
}
