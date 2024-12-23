// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "GameFramework/PlayerState.h"
#include "Engine/EngineBaseTypes.h"

void UAuraAbilitySystemComponent::InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor)
{
	// This is called automatically on server and client on initialize component setting both owner actor and avatar actor to the player state
	// If server calls this, client will call it automatically again on UAbilitySystemComponent::OnRep_OwningActor()
	
	Super::InitAbilityActorInfo(InOwnerActor, InAvatarActor);
	
	const AActor* ActorOwner = GetOwner();
	check(ActorOwner);
	const ENetRole LocalRole = ActorOwner->GetLocalRole();
	const APlayerState* PSOwner = Cast<APlayerState>(ActorOwner);
	
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

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	const AActor* ActorOwner = GetOwner();
	if (!ActorOwner || ActorOwner->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// Server functionality to add abilities
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		// Create ability spec from class
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);

		// Add startup input tag to the abilities dynamic ability tags
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//AbilitySpec.DynamicAbilityTags.AddTag(AuraGameplayAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayAbility->StartupInputTag);
		}

		// Give the ability
		GiveAbility(AbilitySpec);
		// Optional, will give and activate
		//GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}
	
	if (FGameplayAbilitySpec* AbilitySpec = FindAbilityForTag(InputTag))
	{
		AbilitySpecInputPressed(*AbilitySpec);
		
		if (!AbilitySpec->IsActive())
		{
			TryActivateAbility(AbilitySpec->Handle);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid())
	{
		return;
	}

	if (FGameplayAbilitySpec* AbilitySpec = FindAbilityForTag(InputTag))
	{
		AbilitySpecInputReleased(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// Only server is in here
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
			//  If not the servers playerstate then send client RPC to owning client to handle the event
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
	// Broadcast for HUD widget controller, server already checked that the tag container contains "MessageTag"
	EffectAssetTags.Broadcast(TagContainer);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::FindAbilityForTag(const FGameplayTag& InTag)
{
	return GetActivatableAbilities().FindByPredicate([InTag](const FGameplayAbilitySpec& AbilitySpec)
		{ return AbilitySpec.DynamicAbilityTags.HasTagExact(InTag); });
}

void UAuraAbilitySystemComponent::PrintNetModeInfo() const
{
	const AActor* ActorOwner = GetOwner();
	const FString NetModeString = GetNetMode() < NM_Client ? TEXT("Server Net Mode") : TEXT("Client Net Mode");
	const ENetRole LocalRole = GetOwner()->GetLocalRole();
	const APlayerState* PSOwner = Cast<APlayerState>(ActorOwner);
	const FString OwnerName = PSOwner ? PSOwner->GetPlayerName() : ActorOwner->GetName();
	
	UE_LOG(LogTemp, Warning, TEXT("Ability system component - Player: %s, Avatar: %s, Local Role: %s, Net Mode: %s"),
		*OwnerName, *GetAvatarActor()->GetName(), *UEnum::GetValueAsString(LocalRole), *NetModeString);
}
