// Copyright Berkeley Bidwell

#include "UI/WidgetController/AuraWidgetController.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "System/AuraGameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraWidgetController)

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& InWidgetControllerParams)
{
	// This happens one time when the controller is created, before it is set on any widgets
	
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
		Info.Level = AbilitySpec.Level;

		// Both broadcasts use this function to broadcast initial values of abilities
		OnSlotAbilityChanged.Broadcast(Info);
		OnAbilityStatusLevelChanged.Broadcast(AbilityTag, Info.StatusTag, Info.Level);
	});

	// For each activatable ability, will execute lambda above with their ability spec
	ASC->ForEachAbility(BroadcastDelegate);
}

UAbilityInfo* UAuraWidgetController::GetAbilityInfo() const
{
	const UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(GetWorld()->GetGameInstance());
	return AuraGameInstance ? AuraGameInstance->AbilityInfo : nullptr;
}

FAuraAbilityInfo UAuraWidgetController::GetAbilityInfoByAbilityTag(const FGameplayTag& AbilityTag) const
{
	const UAbilityInfo* AbilityInfo = GetAbilityInfo();
	return AbilityInfo ? AbilityInfo->FindAbilityInfo(AbilityTag) : FAuraAbilityInfo();
}

void UAuraWidgetController::OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const
{
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();

	// Clear out the previous slot if it is valid
	if (PrevInputTag.IsValid())
	{
		FAuraAbilityInfo PrevSlotInfo;
		PrevSlotInfo.AbilityTag = AuraGameplayTags.Abilities_None;
		PrevSlotInfo.StatusTag = AuraGameplayTags.Abilities_Status_Unlocked;
		PrevSlotInfo.InputTag = PrevInputTag;
		// Broadcast empty info if prevslot is a valid slot. only if equipping an already-equipped spell
		OnSlotAbilityChanged.Broadcast(PrevSlotInfo);
	}

	// Fill in new slot if valid
	if (InputTag.IsValid())
	{
		if (UAbilityInfo* AbilityInfo = GetAbilityInfo())
		{
			FAuraAbilityInfo Info = AbilityInfo->FindAbilityInfo(AbilityTag);
			Info.StatusTag = Status;
			Info.InputTag = InputTag;
			OnSlotAbilityChanged.Broadcast(Info);
		}	
	}
}
