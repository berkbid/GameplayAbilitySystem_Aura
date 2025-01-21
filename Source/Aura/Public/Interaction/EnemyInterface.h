// Copyright Berkeley Bidwell

#pragma once

#include "UObject/Interface.h"
#include "EnemyInterface.generated.h"

class UObject;

UINTERFACE(MinimalAPI)
class UEnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Used to determine if an actor is an enemy right now
 */
class AURA_API IEnemyInterface
{
	GENERATED_BODY()

public:
	
};
