// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraGA_CastProjectile.generated.h"

class AAuraProjectile;

/**
 * 
 */
UCLASS()
class AURA_API UAuraGA_CastProjectile : public UAuraGameplayAbility
{
	GENERATED_BODY()
	
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** Call to spawn fire bolt actor at combat socket location, only server needs to do this */
	UFUNCTION(BlueprintCallable, Category = "Projectile")
	void CastProjectile();
	
	UPROPERTY(EditDefaultsOnly, BlueprintreadWrite)
	TSubclassOf<AAuraProjectile> AuraProjectileClass;
};
