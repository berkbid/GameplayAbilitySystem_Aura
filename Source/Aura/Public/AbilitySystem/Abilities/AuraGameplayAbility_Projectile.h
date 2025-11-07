// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraGameplayAbility_Projectile.generated.h"

class AAuraProjectile;
class UObject;
struct FGameplayTag;

/**
 * 
 */
UCLASS(MinimalAPI, Abstract)
class UAuraGameplayAbility_Projectile : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
	protected:
	// UGameplayAbility
	AURA_API virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	AURA_API virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	AURA_API virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// ~UGameplayAbility

	/** Call to spawn fire bolt actor at combat socket location, only server needs to do this */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	AURA_API void CastProjectile(const FVector& ProjectileTargetLocation, const FGameplayTag& SpawnLocationSocketTag, bool bOverridePitch = false, float PitchOverride = 0.f);

	/** Updates TargetLocation and the facing target on actor's motion warping component for anim montage to utilize */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	AURA_API void UpdateTargetLocation(const FVector& InTargetLocation);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> AuraProjectileClass;
	
	/** Target location from mouse target data when activating ability */
	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 NumProjectiles = 5;

};
