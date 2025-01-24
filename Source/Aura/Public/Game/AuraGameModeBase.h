// Copyright Berkeley Bidwell

#pragma once

#include "ModularGameMode.h"
#include "AuraGameModeBase.generated.h"

class AActor;
class UObject;

/**
 * AAuraGameModeBase
 *
 *	The base game mode class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class AURA_API AAuraGameModeBase : public AModularGameModeBase
{
	GENERATED_BODY()
	
public:
	AAuraGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
