// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(100.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Registers attribute to be replicated with no conditions, always set on client even if value doesn't change when set (may want to respond to act of setting)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana,		COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// Inform ability system of new replicated value
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Health - Old: %f, New: %f"), OldHealth.GetCurrentValue(), Health.GetCurrentValue());
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_MaxHealth - Old: %f, New: %f"), OldMaxHealth.GetCurrentValue(), MaxHealth.GetCurrentValue());
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Mana - Old: %f, New: %f"), OldMana.GetCurrentValue(), Mana.GetCurrentValue());
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_MaxMana - Old: %f, New: %f"), OldMaxMana.GetCurrentValue(), MaxMana.GetCurrentValue());
}
