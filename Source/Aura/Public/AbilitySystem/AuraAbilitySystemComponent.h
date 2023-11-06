// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	virtual void InitAbilityActorInfo(AActor* InOwnerActor, AActor* InAvatarActor) override;

	/** Call from base character class to add startup abilities */
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);

	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
protected:
	void EffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Reliable, Client)
	void ClientEffectAppliedTags(const FGameplayTagContainer& TagContainer);
	
public:
	FDelegateHandle EffectAppliedDelegate;
	
	FEffectAssetTags EffectAssetTags;

private:
	FGameplayAbilitySpec* FindAbilityForTag(const FGameplayTag& InTag);

	void PrintNetModeInfo() const;
};
