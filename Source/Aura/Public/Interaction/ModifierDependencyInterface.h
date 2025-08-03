// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "UObject/Interface.h"
#include "ModifierDependencyInterface.generated.h"

class UObject;

UINTERFACE(MinimalAPI)
class UModifierDependencyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IModifierDependencyInterface
{
	GENERATED_BODY()

public:
	virtual FOnExternalGameplayModifierDependencyChange* GetOnModifierDependencyChanged() { return &OnLevelUpModifierDependencyChange; }
	
protected:
	/** Broadcast to have MMC's which override GetExternalModifierDependencyMulticast() to update. Ex. MMC_MaxHealth and MMC_MaxMana. */
	FOnExternalGameplayModifierDependencyChange OnLevelUpModifierDependencyChange;
};
