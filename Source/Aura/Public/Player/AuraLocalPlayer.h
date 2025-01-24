// Copyright Berkeley Bidwell

#pragma once

#include "CommonLocalPlayer.h"
#include "AuraLocalPlayer.generated.h"

class UObject;

/**
 * UAuraLocalPlayer
 */
UCLASS()
class AURA_API UAuraLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()
	
public:
	UAuraLocalPlayer();

	//~UObject interface
	virtual void PostInitProperties() override;
	//~End of UObject interface
	
};
