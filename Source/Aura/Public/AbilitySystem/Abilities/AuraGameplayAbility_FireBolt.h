// Copyright Berkeley Bidwell

#pragma once

#include "AuraGameplayAbility_Projectile.h"
#include "AuraGameplayAbility_FireBolt.generated.h"

/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraGameplayAbility_FireBolt : public UAuraGameplayAbility_Projectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	AURA_API void SpawnProjectiles(const FVector& ProjectileLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget = nullptr);
	
	// UAuraGameplayAbility
	AURA_API virtual FString GetDescription(int32 Level) const override;
	AURA_API virtual FString GetNextLevelDescription(int32 Level) const override;
	// ~UAuraGameplayAbility

protected:
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float ProjectileSpread = 90.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	int32 MaxNumProjectiles = 5;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	bool bLaunchHomingProjectiles = true;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float HomingAccelerationMin = 1600.f;
	
	UPROPERTY(EditDefaultsOnly, Category="Aura|FireBolt")
	float HomingAccelerationMax = 3200.f;
	
private:
	FString GetDescriptionInternal(const FString& InTitle, int32 Level) const;

};
