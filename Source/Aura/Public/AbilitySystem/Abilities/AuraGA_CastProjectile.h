// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraGA_CastProjectile.generated.h"

class AAuraProjectile;
class UGameplayEffect;
class UObject;

/**
 * 
 */
UCLASS()
class AURA_API UAuraGA_CastProjectile : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
protected:
	// UGameplayAbility
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	// ~UGameplayAbility
	
	/** Call to spawn fire bolt actor at combat socket location, only server needs to do this */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void CastProjectile(const FVector& ProjectileTargetLocation);

	/** Updates TargetLocation and the facing target on actor's motion warping component for anim montage to utilize */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void UpdateTargetLocation(const FVector& InTargetLocation);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<AAuraProjectile> AuraProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	/** Target location from mouse target data when activating ability */
	UPROPERTY(BlueprintReadOnly)
	FVector TargetLocation = FVector::ZeroVector;
};
