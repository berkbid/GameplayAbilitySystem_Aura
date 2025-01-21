// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

class AActor;
class UGameplayAbility;
class UObject;
struct FGameplayTag;
struct FGameplayTagContainer;
struct FGameplayEffectSpec;
struct FActiveGameplayEffectHandle;
struct FGameplayAbilitySpec;

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	UAuraAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	/** Call from base character class to add startup abilities */
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	/** Currently only called if TagContainer contains MessageTag */
	UFUNCTION(Reliable, Client)
	void ClientEffectAppliedTags(const FGameplayTagContainer& TagContainer);
	
public:
	/** Broadcast when effect is applied with a MessageTag Gameplay Tag */
	FEffectAssetTags EffectAssetTags;

private:
	FDelegateHandle EffectAppliedDelegate;
	
	FGameplayAbilitySpec* FindAbilityForTag(const FGameplayTag& InTag);

	void PrintNetModeInfo() const;
};
