// Copyright Berkeley Bidwell

#pragma once

#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class AController;
class AAuraCharacter;
class AAuraPlayerController;
class UObject;
class UAuraAbilitySystemComponent;
struct FGameplayTag;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEffectSpec;
struct FGameplayEventData;

/**
 * UAuraGameplayAbility
 *
 *	The base gameplay ability class used by this project.
 */
UCLASS(MinimalAPI, Abstract, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	AURA_API UAuraGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Aura|Ability")
	AURA_API UAuraAbilitySystemComponent* GetAuraAbilitySystemComponentFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Aura|Ability")
	AURA_API AAuraPlayerController* GetAuraPlayerControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Aura|Ability")
	AURA_API AController* GetControllerFromActorInfo() const;

	UFUNCTION(BlueprintCallable, Category = "Aura|Ability")
	AURA_API AAuraCharacter* GetAuraCharacterFromActorInfo() const;

	/** Get the ability gameplay tag for this ability */
	UFUNCTION(BlueprintPure, Category = "Aura|Ability")
	AURA_API FGameplayTag GetAbilityTag() const;
	
	AURA_API virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

public:
	/** Use when giving abilities to character */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;
	
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Cost")
	FScalableFloat Cost;
	
protected:
	// UGameplayAbility interface
	// AURA_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	// AURA_API virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	// AURA_API virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	// AURA_API virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	AURA_API virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// AURA_API virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	// AURA_API virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	// AURA_API virtual UGameplayEffect* GetCostGameplayEffect() const override;
	// AURA_API virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	// AURA_API virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	// AURA_API virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	// ~UGameplayAbility interface
};
