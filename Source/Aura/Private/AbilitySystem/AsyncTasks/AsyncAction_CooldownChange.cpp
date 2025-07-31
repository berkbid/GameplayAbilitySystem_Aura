// Copyright Berkeley Bidwell

#include "AbilitySystem/AsyncTasks/AsyncAction_CooldownChange.h"
#include "AbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_CooldownChange)

UAsyncAction_CooldownChange* UAsyncAction_CooldownChange::WaitForCooldownChange(UAbilitySystemComponent* InASC, const FGameplayTag& InCooldownTag)
{
	if (!IsValid(InASC) || !InCooldownTag.IsValid())
	{
		return nullptr;
	}
	
	UAsyncAction_CooldownChange* Action = NewObject<UAsyncAction_CooldownChange>();
	Action->RegisterWithGameInstance(InASC);

	if (Action->IsRegistered())
	{
		Action->ASC = InASC;
		Action->CooldownTag = InCooldownTag;
	}
	else
	{
		Action->SetReadyToDestroy();
	}

	return Action;
}

void UAsyncAction_CooldownChange::Activate()
{
	UAbilitySystemComponent* AbilitySystemComponent = ASC.Get();
	if (AbilitySystemComponent && CooldownTag.IsValid())
	{
		ASC->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &UAsyncAction_CooldownChange::CooldownTagChanged);
	}
	else
	{
		SetReadyToDestroy();
	}
}

void UAsyncAction_CooldownChange::SetReadyToDestroy()
{
	Super::SetReadyToDestroy();
	
	UAbilitySystemComponent* AbilitySystemComponent = ASC.Get();
	if (AbilitySystemComponent && CooldownTag.IsValid())
	{
		AbilitySystemComponent->RegisterGameplayTagEvent(CooldownTag, EGameplayTagEventType::NewOrRemoved).RemoveAll(this);
	}
	
	ASC.Reset();
}

void UAsyncAction_CooldownChange::CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount)
{
	if (NewCount == 0)
	{
		CooldownEnd.Broadcast(0.f);
	}
	else
	{
		UAbilitySystemComponent* AbilitySystemComponent = ASC.Get();
		if (!AbilitySystemComponent || !CooldownTag.IsValid())
		{
			return;
		}
		
		FGameplayEffectQuery GameplayEffectQuery = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(CooldownTag.GetSingleTagContainer());
		const TArray<float> EffectsTimeRemaining = AbilitySystemComponent->GetActiveEffectsTimeRemaining(GameplayEffectQuery);
	
		if (!EffectsTimeRemaining.IsEmpty())
		{
			const float TimeRemaining = FMath::Max(EffectsTimeRemaining);
			CooldownStart.Broadcast(TimeRemaining);
		}
	}
}
