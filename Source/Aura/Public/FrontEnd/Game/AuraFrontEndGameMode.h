// Copyright Berkeley Bidwell

#pragma once

#include "ModularGameMode.h"
#include "AuraFrontEndGameMode.generated.h"

class AActor;
class UObject;

/**
 * AAuraFrontEndGameMode
 *
 *	The front end game mode class used by this project.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The front end game mode class used by this project."))
class AURA_API AAuraFrontEndGameMode : public AModularGameModeBase
{
	GENERATED_BODY()
	
public:
	AAuraFrontEndGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
