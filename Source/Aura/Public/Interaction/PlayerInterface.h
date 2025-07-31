// Copyright Berkeley Bidwell

#pragma once

#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

class UObject;

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IPlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent)
	void AddToXp(int32 InXp);
	
};
