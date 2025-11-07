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
	// UAuraGameplayAbility
	AURA_API virtual FString GetDescription(int32 Level) const override;
	AURA_API virtual FString GetNextLevelDescription(int32 Level) const override;
	// ~UAuraGameplayAbility

private:
	FString GetDescriptionInternal(const FString& InTitle, int32 Level) const;

};
