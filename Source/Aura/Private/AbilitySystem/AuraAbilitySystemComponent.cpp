// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraGameplayTags.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraLogChannels.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Interaction/PlayerInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAbilitySystemComponent)

enum ENetRole : int;

UAuraAbilitySystemComponent::UAuraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

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

	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	// Server functionality to add abilities
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupAbilities)
	{
		// Create ability spec from class, give at level 1
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);

		// Add startup input tag to the abilities dynamic ability tags
		if (const UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AuraGameplayAbility->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
		}

		// Give the ability
		GiveAbility(AbilitySpec);
		// Optional, will give and activate
		//GiveAbilityAndActivateOnce(AbilitySpec);
	}
	
	bStartupAbilitiesGiven = true;
	OnAbilitiesGiven.Broadcast();
}

void UAuraAbilitySystemComponent::AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	const AActor* ActorOwner = GetOwner();
	if (!ActorOwner || ActorOwner->GetLocalRole() < ROLE_Authority)
	{
		return;
	}

	// Server functionality to add abilities
	for (const TSubclassOf<UGameplayAbility>& AbilityClass : StartupPassiveAbilities)
	{
		// Create ability spec from class, give at level 1
		FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
		
		// Give the ability and activate
		GiveAbilityAndActivateOnce(AbilitySpec);
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
		// TODO: Interesting that we processed a pressed event every tick while input tag is held
		AbilitySpecInputPressed(*AbilitySpec);

		// This keeps the ability from being activated/cancelled if it is already active
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

void UAuraAbilitySystemComponent::ForEachAbility(const FForEachAbility& Delegate)
{
	ABILITYLIST_SCOPE_LOCK();
	for (const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (!Delegate.ExecuteIfBound(AbilitySpec))
		{
			UE_LOG(LogAuraAbilitySystem, Error, TEXT("Failed to execute delegate in %hs"), __FUNCTION__);
		}
	}
}

FGameplayTag UAuraAbilitySystemComponent::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if (AbilitySpec.Ability)
	{
		for (const FGameplayTag& Tag : AbilitySpec.Ability->GetAssetTags())
		{
			if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))))
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for (const FGameplayTag& Tag : AbilitySpec.GetDynamicSpecSourceTags())
	{
		if (Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status"))))
		{
			return Tag;
		}
	}
	return FGameplayTag();
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (!AbilityTag.IsValid())
	{
		return nullptr;
	}
	
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.Ability && AbilitySpec.Ability->GetAssetTags().HasTagExact(AbilityTag))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

FGameplayTag UAuraAbilitySystemComponent::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayTag UAuraAbilitySystemComponent::GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

void UAuraAbilitySystemComponent::AddOrRefundAttribute(const FGameplayTag& AttributeTag, int32 IncrementAmount)
{
	if (IncrementAmount == 0) { return; }

	if (!GetOwnerActor()->HasAuthority())
	{
		// Check with the attribute set if we are able to apply the given attribute point amount
		if (const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(GetAttributeSet(UAuraAttributeSet::StaticClass())))
		{
			// Client does client-side check of valid application of attribute points
			// And server will also do the check on server-side just in case
			if (AuraAttributeSet->CanApplyAttributePoints(AttributeTag, IncrementAmount))
			{
				ServerAddOrRefundAttribute(AttributeTag, IncrementAmount);
			}
		}
	}
	else
	{
		// Server will check if valid application of attribute points
		ServerAddOrRefundAttribute(AttributeTag, IncrementAmount);
	}
}

void UAuraAbilitySystemComponent::UpdateAbilitiesEligibilityFromLevelUp(int32 Level)
{
	//TODO: Could check HasAuthority()
	//GetOwner()->HasAuthority()
	
	UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	check(AbilityInfo);

	const FGameplayTag Eligible = FAuraGameplayTags::Get().Abilities_Status_Eligible;
	
	for (const FAuraAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		// If the level requirement is not met, or the ability has already been given
		if (Level < Info.LevelRequirement || GetSpecFromAbilityTag(Info.AbilityTag))
		{
			continue;
		}

		// TODO: Make sure we aren't giving ability multiple times
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(Eligible);
		GiveAbility(AbilitySpec);

		// TODO: Maybe giveability does this already
		// Forces ability spec to replicate now
		//MarkAbilitySpecDirty(AbilitySpec);

		// The new ability status will be broadcast for local controller on OnGiveAbility() once it is officially in activatableabilities array
	}
}

void UAuraAbilitySystemComponent::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag)
{
	if (FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FGameplayTag& PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		
		// Return if ability already in correct slot
		if (PrevSlot.MatchesTagExact(InputTag))
		{
			return;
		}
		
		const FGameplayTag& Status = GetStatusTagFromSpec(*AbilitySpec);
		
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		const bool bAbilityIsEquipped = Status.MatchesTagExact(GameplayTags.Abilities_Status_Equipped);
		const bool bAbilityIsUnlocked = Status.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked);
		
		const bool bEquipValid = bAbilityIsEquipped || bAbilityIsUnlocked;
		
		if (bEquipValid)
		{
			// Remove this InputTag (slot) from any ability that has it
			// Clear this slot of any ability in it
			ClearAbilitiesOfSlot(InputTag);
			
			// Clear this ability's slot, in case it is a different slot
			ClearSlot(AbilitySpec);
			
			// Now assign this slot to this ability
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(InputTag);
			if (bAbilityIsUnlocked)
			{
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Unlocked);
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			
			MarkAbilitySpecDirty(*AbilitySpec);
			
			// Call client rpc to broadcast new ability status and input tag
			ClientEquipAbility(AbilityTag, Status, InputTag, PrevSlot);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag)
{
	OnAbilityEquipped.Broadcast(AbilityTag, Status, InputTag, PrevInputTag);
}

bool UAuraAbilitySystemComponent::GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, const int32 Level, FString& OutDescription, FString& OutNextLevelDescription)
{
	if (!AbilityTag.IsValid() ||
		!AbilityTag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities"))) ||
		AbilityTag.MatchesTagExact(FAuraGameplayTags::Get().Abilities_None))
	{
		OutDescription = TEXT("");
		OutNextLevelDescription = TEXT("");
		return false;
	}
	
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if (UAuraGameplayAbility* AuraGameplayAbility = Cast<UAuraGameplayAbility>(AbilitySpec->Ability))
		{
			// Not using AbilitySpec->Level as it will not be valid on client yet
			OutDescription = AuraGameplayAbility->GetDescription(Level);
			OutNextLevelDescription = AuraGameplayAbility->GetNextLevelDescription(Level + 1);
			return true;
		}
	}
	
	// Ability is locked because did not find it above
	if (const UAbilityInfo* AbilityInfo = UAuraAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor()))
	{
		OutDescription = UAuraGameplayAbility::GetLockedDescription(AbilityInfo->FindAbilityInfo(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return false;
}

void UAuraAbilitySystemComponent::ClearSlot(FGameplayAbilitySpec* AbilitySpec)
{
	if (AbilitySpec)
	{
		const FGameplayTag& Slot = GetInputTagFromSpec(*AbilitySpec);
		AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(Slot);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::ClearAbilitiesOfSlot(const FGameplayTag& InputTag)
{
	ABILITYLIST_SCOPE_LOCK();
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilityHasSlot(&AbilitySpec, InputTag))
		{
			ClearSlot(&AbilitySpec);
		}
		
		/*
		if (GetInputTagFromSpec(AbilitySpec) == InputTag)
		{
			ClearSlot(&AbilitySpec);
		}
		*/
	}
}

bool UAuraAbilitySystemComponent::AbilityHasSlot(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Slot)
{
	if (AbilitySpec)
	{
		for (const FGameplayTag& Tag : AbilitySpec->GetDynamicSpecSourceTags())
		{
			if (Tag.MatchesTagExact(Slot))
			{
				return true;
			}
		}
	}
	return false;
}

void UAuraAbilitySystemComponent::ServerSpendOrRefundSpellPoint_Implementation(const FGameplayTag& AbilityTag, int32 Amount)
{
	FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag);
	AActor* Avatar = GetAvatarActor();
	
	if (Amount != 0 && AbilitySpec && Avatar && Avatar->Implements<UPlayerInterface>() && IPlayerInterface::Execute_SpendOrRefundSpellPoints(Avatar, AbilityTag, Amount))
	{
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
		
		FGameplayTag AbilityStatus = GetStatusTagFromSpec(*AbilitySpec);
		
		// We are spending spell points, // we know it's not status locked, either eligible, equipped, or unlocked
		if (Amount > 0)
		{
			
			if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Eligible))
			{
				// Change status, remove old tag, add new
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
				AbilityStatus = GameplayTags.Abilities_Status_Unlocked;
			}
			else if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped) || AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Unlocked))
			{
				// Change the ability level, status stays the same
				
				// This will not cancel it if it is already active (if we want that need to remove and re-add ability)
				AbilitySpec->Level += Amount;
			}
		}
		else // We are refunding spell points, we know status is either unlocked or equipped
		{
			const int32 NewAbilityLevel = AbilitySpec->Level + Amount;
			
			if (NewAbilityLevel < 0)
			{
				// Log error
				return;
			}

			const bool bNewAbilityLevelIsZero = NewAbilityLevel == 0;
			
			AbilitySpec->Level = bNewAbilityLevelIsZero ? 1 : NewAbilityLevel;
			
			// Cannot go to 0, when lowering to level 0, we keep it at level 1 but change status instead. Going to eligible status
			if (bNewAbilityLevelIsZero)
			{
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(AbilityStatus);
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Eligible);
				
				if (AbilityStatus.MatchesTagExact(GameplayTags.Abilities_Status_Equipped))
				{
					//ServerEquipAbility(AbilityTag, GetInputTagFromSpec(*AbilitySpec));
					const FGameplayTag& PrevInputTag = GetInputTagFromSpec(*AbilitySpec);
					ClearSlot(AbilitySpec);
					
					// TODO: Should we clear ability also? Would need access to ability spec handle
					//ClearAbility(AbilitySpec->Handle);
					
					// Not sure if this is the best solution, ClientUpdateAbilityStatusAndLevel isn't working because doesn't send previous input tag
					ClientEquipAbility(AbilityTag, AbilityStatus, FGameplayTag(), PrevInputTag);
				}
				else // Unlocked
				{

				}

				AbilityStatus = GameplayTags.Abilities_Status_Eligible;
			}
		}
		
		// Because we changed our ability spec, we can force it to replicate now instead of on the next update, do this before or after broadcast below?
		MarkAbilitySpecDirty(*AbilitySpec);

		// Broadcast the ability's new status and level
		ClientUpdateAbilityStatusAndLevel(AbilityTag, AbilityStatus, AbilitySpec->Level);
	}
}

void UAuraAbilitySystemComponent::ServerAddOrRefundAttribute_Implementation(const FGameplayTag& AttributeTag, int32 IncrementAmount)
{
	AActor* Avatar = GetAvatarActor();
	if (Avatar && Avatar->Implements<UPlayerInterface>())
	{
		// Making sure that we successfully increment or decrement the attribute before sending gameplay event
		if (IPlayerInterface::Execute_AddOrRefundAttributePoints(Avatar, AttributeTag, IncrementAmount))
		{
			FGameplayEventData Payload;
			Payload.EventTag = AttributeTag;
			Payload.EventMagnitude = IncrementAmount;
			
			// GA_ListenForEvents will listen for this and apply GE to self which will add IncrementAmount of strength attribute to the attribute set
			UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
		}
	}
}

void UAuraAbilitySystemComponent::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();

	if (!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		OnAbilitiesGiven.Broadcast();
	}
}

void UAuraAbilitySystemComponent::OnGiveAbility(FGameplayAbilitySpec& AbilitySpec)
{
	Super::OnGiveAbility(AbilitySpec);

	// Broadcasting stuff for UI, only local controller needs it
	if (AbilityActorInfo.IsValid() && AbilityActorInfo->IsLocallyControlledPlayer())
	{
		OnAbilityStatusChanged.Broadcast(GetAbilityTagFromSpec(AbilitySpec), GetStatusTagFromSpec(AbilitySpec), AbilitySpec.Level);
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

void UAuraAbilitySystemComponent::ClientUpdateAbilityStatusAndLevel_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel)
{
	OnAbilityStatusChanged.Broadcast(AbilityTag, StatusTag, AbilityLevel);
}

void UAuraAbilitySystemComponent::ClientEffectAppliedTags_Implementation(const FGameplayTagContainer& TagContainer)
{
	// Broadcast for HUD widget controller, server already checked that the tag container contains "MessageTag"
	EffectAssetTags.Broadcast(TagContainer);
}

FGameplayAbilitySpec* UAuraAbilitySystemComponent::FindAbilityForTag(const FGameplayTag& InTag)
{
	return GetActivatableAbilities().FindByPredicate([InTag](const FGameplayAbilitySpec& AbilitySpec)
		{ return AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InTag); });
}
