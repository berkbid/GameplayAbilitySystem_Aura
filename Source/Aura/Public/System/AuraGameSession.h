// Copyright Berkeley Bidwell

#pragma once

#include "GameFramework/GameSession.h"
#include "AuraGameSession.generated.h"

class UObject;

/**
 * 
 */
UCLASS(Config = Game)
class AURA_API AAuraGameSession : public AGameSession
{
	GENERATED_BODY()
	
public:
	AAuraGameSession(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:

	/** Override to disable the default behavior */
	virtual bool ProcessAutoLogin() override;

	virtual void HandleMatchHasStarted() override;
	virtual void HandleMatchHasEnded() override;
	
};
