// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// This is called automatically on server and client on initialize component setting both owner actor and avatar actor to the player state
	// If server calls this, client will call it automatically again on UAbilitySystemComponent::OnRep_OwningActor()

	const AActor* OwnerActor = GetOwner();
	check(OwnerActor);
	const ENetRole LocalRole = OwnerActor->GetLocalRole();
	const APlayerState* PSOwner = Cast<APlayerState>(OwnerActor);
	const FString OwnerName = PSOwner? PSOwner->GetPlayerName() : OwnerActor->GetName();
	
	UE_LOG(LogTemp, Warning, TEXT("Player: %s, init owner actor: %s, Avatar actor: %s, Local Role: %s"),
		*OwnerName, *InOwnerActor->GetName(), *InAvatarActor->GetName(), *UEnum::GetValueAsString(LocalRole));
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);

	// Delegate is only called on server, so only binding on server
	if (LocalRole == ROLE_Authority)
	{
		// Only want to bind to delegate once
		if (!EffectAppliedDelegate.IsValid())
		{
			EffectAppliedDelegate = OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
		}
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	const ENetRole LocalRole = GetOwner()->GetLocalRole();
	UE_LOG(LogTemp, Warning, TEXT("Effect applied to avatar: %s, for Local Role: %s"), *GetAvatarActor()->GetName(), *UEnum::GetValueAsString(LocalRole));
}
