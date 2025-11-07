// Copyright Berkeley Bidwell

#include "UI/WidgetController/SpellMenuWidgetController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Player/AuraPlayerState.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(SpellMenuWidgetController)

void USpellMenuWidgetController::BindCallBacksToDependencies()
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	
	ASC->OnAbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 NewLevel)
	{
		if (OnAbilityStatusLevelChanged.IsBound())
		{
			OnAbilityStatusLevelChanged.Broadcast(AbilityTag, StatusTag, NewLevel);
		}
	});
	
	ASC->OnAbilityEquipped.AddUObject(this, &UAuraWidgetController::OnAbilityEquipped);
	
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(WidgetControllerParams.PlayerState);
	AuraPS->OnSpellPointUpdate.AddLambda([this](int32 NewSpellPoints)
	{
		if (OnSpellPointsChanged.IsBound()) { OnSpellPointsChanged.Broadcast(NewSpellPoints); }
	});
}

void USpellMenuWidgetController::BroadcastInitialValues()
{
	// TODO: Could do like HUDWidgetController, unless we assume startup abilities are already given
	// Broadcast initial ability information values (which are active, and their status)
	BroadcastAbilityInfo();
	
	// Broadcast initial spell points value
	AAuraPlayerState* AuraPS = CastChecked<AAuraPlayerState>(WidgetControllerParams.PlayerState);
	if (OnSpellPointsChanged.IsBound()) { OnSpellPointsChanged.Broadcast(AuraPS->GetSpellPoints()); }
}

void USpellMenuWidgetController::SpendOrRefundSpellPoint(const FGameplayTag& AbilityTag, int32 SpellPoints) const
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	check(ASC);
	
	// Spend a spell point
	ASC->ServerSpendOrRefundSpellPoint(AbilityTag, SpellPoints);
}

bool USpellMenuWidgetController::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, const int32 Level, FString& OutDescription, FString& OutNextLevelDescription)
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	check(ASC);
	return ASC->GetDescriptionsByAbilityTag(AbilityTag, Level, OutDescription, OutNextLevelDescription);
}

void USpellMenuWidgetController::EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(WidgetControllerParams.AbilitySystemComponent);
	check(ASC);
	ASC->ServerEquipAbility(AbilityTag, InputTag);
}
