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
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitiesGiven, UAuraAbilitySystemComponent*);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);

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

	/** Can be called by server or client, will call server RPC for functionality */
	AURA_API void AddOrRefundAttribute(const FGameplayTag& AttributeTag, int32 IncrementAmount);

public:
	/** Broadcast when effect is applied with a MessageTag Gameplay Tag */
	FEffectAssetTags EffectAssetTags;
	
	/** Broadcast when an ability is given */
	FAbilitiesGiven OnAbilitiesGiven;
	
protected:
	AURA_API virtual void OnRep_ActivateAbilities() override;
	
	AURA_API void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	/** Currently only called if TagContainer contains MessageTag */
	UFUNCTION(Reliable, Client)
	AURA_API void ClientEffectAppliedTags(const FGameplayTagContainer& TagContainer);

private:
	UFUNCTION(Reliable, Server)
	void ServerAddOrRefundAttribute(const FGameplayTag& AttributeTag, int32 IncrementAmount);
	
	FGameplayAbilitySpec* FindAbilityForTag(const FGameplayTag& InTag);

	void PrintNetModeInfo() const;

private:
	FDelegateHandle EffectAppliedDelegate;
};
