// Copyright Berkeley Bidwell

#pragma once

#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

class UObject;
struct FGameplayTag;

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	/** Use when giving abilities to character */
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Damage")
	FScalableFloat Damage;
};
