// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"
#include "Engine/EngineBaseTypes.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// This is called automatically on server and client on initialize component setting both owner actor and avatar actor to the player state
	// If server calls this, client will call it automatically again on UAbilitySystemComponent::OnRep_OwningActor()
	
	const AActor* ActorOwner = GetOwner();
	check(ActorOwner);
	const FString NetModeString = GetNetMode() < NM_Client ? TEXT("Server Net Mode") : TEXT("Client Net Mode");
	const ENetRole LocalRole = ActorOwner->GetLocalRole();
	const APlayerState* PSOwner = Cast<APlayerState>(ActorOwner);
	const FString OwnerName = PSOwner? PSOwner->GetPlayerName() : ActorOwner->GetName();
	
	UE_LOG(LogTemp, Warning, TEXT("Player: %s, init owner actor: %s, Avatar actor: %s, Local Role: %s, Net Mode: %s"),
		*OwnerName, *InOwnerActor->GetName(), *InAvatarActor->GetName(), *UEnum::GetValueAsString(LocalRole), *NetModeString);
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	// Delegate is only called on server, so only binding on server
	// Only binding if owner of this ASC is a playerstate (Not important for AI which has ASC on character)
	
	if (LocalRole == ROLE_Authority && PSOwner)
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
	// Only server is in here
	const FString NetModeString = GetNetMode() < NM_Client ? TEXT("Server Net Mode") : TEXT("Client Net Mode");
	const ENetRole LocalRole = GetOwner()->GetLocalRole();
	UE_LOG(LogTemp, Warning, TEXT("Effect applied to avatar: %s, for Local Role: %s, Net Mode: %s"),
		*GetAvatarActor()->GetName(), *UEnum::GetValueAsString(LocalRole), *NetModeString);

	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);
	
	const FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
	
	if (!TagContainer.HasTag(MessageTag))
	{
		return;
	}

	// Only want to broadcast if this is attached to a playerstate
	if (const APlayerState* PS = Cast<APlayerState>(GetOwner()))
	{
		if (const AController* Controller = PS->GetOwningController())
		{
			if (!Controller->IsLocalController())
			{
				ClientEffectAppliedTags(TagContainer);
			}
			else
			{
				// Call functionality directly on server
				ClientEffectAppliedTags_Implementation(TagContainer);
			}
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectAppliedTags_Implementation(const FGameplayTagContainer& TagContainer)
{
	const ENetRole LocalRole = GetOwner()->GetLocalRole();
	const FString NetModeString = GetNetMode() < NM_Client ? TEXT("Server Net Mode") : TEXT("Client Net Mode");
	
	UE_LOG(LogTemp, Warning, TEXT("ClientEffectAppliedTags() for Local Role: %s, Net Mode: %s"),
		*UEnum::GetValueAsString(LocalRole), *NetModeString);
	
	// Broadcast for widgetcontroller
	EffectAssetTags.Broadcast(TagContainer);
}