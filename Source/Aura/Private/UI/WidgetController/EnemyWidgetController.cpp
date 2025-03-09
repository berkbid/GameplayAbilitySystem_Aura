// Copyright Berkeley Bidwell

#include "UI/WidgetController/EnemyWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EnemyWidgetController)

void UEnemyWidgetController::BroadcastInitialValues()
{
	const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);

	UE_LOG(LogTemp, Warning, TEXT("Broadcasting initial enemy values"));
	
	OnHealthChanged.Broadcast(AttributeSet->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth());
}

void UEnemyWidgetController::BindCallBacksToDependencies()
{
	check(WidgetControllerParams.AbilitySystemComponent);

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
	}
}
