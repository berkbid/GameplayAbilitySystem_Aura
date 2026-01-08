// Copyright Berkeley Bidwell

#include "UI/WidgetController/HUDWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "Player/AuraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HUDWidgetController)

void UHUDWidgetController::BindCallBacksToDependencies()
{
	// This is called one time after the widget controller params are set, before the HUD is added to viewport
	
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);
	
	UE_LOG(LogTemp, Warning, TEXT("Binding callbacks to dependencies on HUDWidgetController"));
	
	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		if (OnHealthChanged.IsBound()) { OnHealthChanged.Broadcast(Data.NewValue); }
	});
	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		if (OnMaxHealthChanged.IsBound()) { OnMaxHealthChanged.Broadcast(Data.NewValue); }
	});
	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		if (OnManaChanged.IsBound())
		{
			// TODO: This being called twice(once) on server, 3 times on client
			// Ex. for client, if going from 80 to 60 mana. We receive 60, then 80, then 60 again. Like predicted, previous, real.
				// First from commitability(), second from postnetreceive calls attributeagregatordirty, third from Onrep_Mana.
			// Ex. for server, if going from 80 to 60 mana. We receive 60, then 60. Unless remove setmana call in PostGameplayEffectExecute, then receive once
				// Receive broadcast from commitability()
			UE_LOG(LogTemp, Warning, TEXT("Mana attribute changed: %f"), Data.NewValue);
			OnManaChanged.Broadcast(Data.NewValue);
		}
	});
	ASC->GetGameplayAttributeValueChangeDelegate(AttributeSet->GetMaxManaAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
	{
		if (OnMaxManaChanged.IsBound()) { OnMaxManaChanged.Broadcast(Data.NewValue); }
	});
	ASC->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& AssetTags)
	{
		if (!MessageWidgetRow.IsBound())
		{
			return;
		}
		
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
	});
	ASC->AbilityCommittedCallbacks.AddLambda([this](UGameplayAbility* Ability)
	{
		if (OnAbilityCommitted.IsBound())
		{
			OnAbilityCommitted.Broadcast(Ability);
		}
	});
	
	ASC->OnAbilityEquipped.AddUObject(this, &UAuraWidgetController::OnAbilityEquipped);
	
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(WidgetControllerParams.PlayerState);
	
	// Experience changed
	AuraPS->OnXpUpdate.AddUObject(this, &UHUDWidgetController::OnXpChanged);

	// Level changed
	AuraPS->OnLevelUpdate.AddLambda([this](int32 NewLevel)
	{
		if (OnPlayerLevelChangedDelegate.IsBound()) { OnPlayerLevelChangedDelegate.Broadcast(NewLevel); }
	});
}

void UHUDWidgetController::BroadcastInitialValues()
{
	// This is called directly after a widget has SetWidgetController called on it and it received WidgetControllerSet event
	const UAuraAttributeSet* AttributeSet = CastChecked<UAuraAttributeSet>(WidgetControllerParams.AttributeSet);

	UE_LOG(LogTemp, Warning, TEXT("Broadcasting initial HUD values"));
	
	if (OnHealthChanged.IsBound()) { OnHealthChanged.Broadcast(AttributeSet->GetHealth()); }
	if (OnMaxHealthChanged.IsBound()) { OnMaxHealthChanged.Broadcast(AttributeSet->GetMaxHealth()); }
	if (OnManaChanged.IsBound()) { OnManaChanged.Broadcast(AttributeSet->GetMana()); }
	if (OnMaxManaChanged.IsBound()) { OnMaxManaChanged.Broadcast(AttributeSet->GetMaxMana()); }

	const AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(WidgetControllerParams.PlayerState);
	OnXpChanged(AuraPS->GetXp());

	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	if (ASC->bStartupAbilitiesGiven)
	{
		BroadcastAbilityInfo();
	}
	else
	{
		ASC->OnAbilitiesGiven.AddUObject(this, &UAuraWidgetController::BroadcastAbilityInfo);	
	}

	// TODO: Broadcast initial OnAbilityCommitted values. Not sure if that is possible
}

void UHUDWidgetController::OnXpChanged(int32 NewXp) const
{
	if (!OnXpPercentChanged.IsBound())
	{
		return;
	}
	
	const AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(WidgetControllerParams.PlayerState);
	const ULevelUpInfo* LevelUpInfo = AuraPS->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("LevelUpInfo is null. Please fill out AuraPlayerState Blueprint"));
	
	OnXpPercentChanged.Broadcast(LevelUpInfo->FindXpPercentForXp(NewXp));
}
