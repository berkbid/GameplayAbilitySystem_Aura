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
UCLASS(MinimalAPI, Config = Game, Meta = (ShortTooltip = "The base game mode class used by this project."))
class AAuraGameModeBase : public AModularGameModeBase
{
	GENERATED_BODY()
	
public:
	AURA_API AAuraGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
};
