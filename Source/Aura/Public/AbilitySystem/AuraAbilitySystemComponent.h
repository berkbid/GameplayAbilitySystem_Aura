// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class UAuraAbilitySystemComponent;
class AActor;
class UGameplayAbility;
class UObject;
struct FGameplayTag;
struct FGameplayTagContainer;
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpec;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE(FAbilitiesGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FAbilityStatusChanged, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, int32 /*AbilityLevel*/);
DECLARE_MULTICAST_DELEGATE_FourParams(FAbilityEquipped, const FGameplayTag& /*AbilityTag*/, const FGameplayTag& /*StatusTag*/, const FGameplayTag& /*InputTag*/, const FGameplayTag& /*PrevInputTag*/);

/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	AURA_API UAuraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	AURA_API virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	/** Call from base character class to add startup abilities */
	AURA_API void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	AURA_API void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	// Bool managed separate on server and client
	bool bStartupAbilitiesGiven = false;
	
	AURA_API void AbilityInputTagHeld(const FGameplayTag& InputTag);
	AURA_API void AbilityInputTagReleased(const FGameplayTag& InputTag);

	AURA_API void ForEachAbility(const FForEachAbility& Delegate);
	
	static AURA_API FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static AURA_API FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static AURA_API FGameplayTag GetStatusTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	AURA_API FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	AURA_API FGameplayTag GetStatusFromAbilityTag(const FGameplayTag& AbilityTag);
	AURA_API FGameplayTag GetInputTagFromAbilityTag(const FGameplayTag& AbilityTag);
	
	/** Can be called by server or client, will call server RPC for functionality */
	AURA_API void AddOrRefundAttribute(const FGameplayTag& AttributeTag, int32 IncrementAmount);

	/** Called from server to update the eligibility of an ability based on player leveling up */
	AURA_API void UpdateAbilitiesEligibilityFromLevelUp(int32 Level);

	UFUNCTION(Reliable, Server)
	AURA_API void ServerSpendOrRefundSpellPoint(const FGameplayTag& AbilityTag, int32 Amount);

	UFUNCTION(Reliable, Server)
	AURA_API void ServerEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);
	
	UFUNCTION(Reliable, Client)
	AURA_API void ClientEquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag);
	
	AURA_API bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, const int32 Level, FString& OutDescription, FString& OutNextLevelDescription);

	AURA_API void ClearSlot(FGameplayAbilitySpec* AbilitySpec);
	AURA_API void ClearAbilitiesOfSlot(const FGameplayTag& InputTag);
	static AURA_API bool AbilityHasSlot(FGameplayAbilitySpec* AbilitySpec, const FGameplayTag& Slot);
	
public:
	/** Broadcast when effect is applied with a MessageTag Gameplay Tag */
	FEffectAssetTags EffectAssetTags;
	
	/** Broadcast when an ability is given */
	FAbilitiesGiven OnAbilitiesGiven;

	/** Broadcast when the status and/or level of an ability changes */
	FAbilityStatusChanged OnAbilityStatusChanged;
	
	/** Broadcast when an ability is equipped or changed slots */
	FAbilityEquipped OnAbilityEquipped;
	
protected:
	// ~UAbilitySystemComponent
	AURA_API virtual void OnRep_ActivateAbilities() override;
	AURA_API virtual void OnGiveAbility(FGameplayAbilitySpec& AbilitySpec) override;
	// ~End UAbilitySystemComponent
	
	AURA_API void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	/** Currently only called if TagContainer contains MessageTag */
	UFUNCTION(Reliable, Client)
	AURA_API void ClientEffectAppliedTags(const FGameplayTagContainer& TagContainer);
	
	UFUNCTION(Reliable, Client)
	AURA_API void ClientUpdateAbilityStatusAndLevel(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag, int32 AbilityLevel);

private:
	UFUNCTION(Reliable, Server)
	void ServerAddOrRefundAttribute(const FGameplayTag& AttributeTag, int32 IncrementAmount);
	
	FGameplayAbilitySpec* FindAbilityForTag(const FGameplayTag& InTag);

private:
	FDelegateHandle EffectAppliedDelegate;
};
