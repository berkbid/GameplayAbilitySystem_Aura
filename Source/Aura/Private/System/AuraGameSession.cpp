// Copyright Berkeley Bidwell

#include "System/AuraGameSession.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameSession)

AAuraGameSession::AAuraGameSession(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

bool AAuraGameSession::ProcessAutoLogin()
{
	// This is actually handled in LyraGameMode::TryDedicatedServerLogin
	//return true;
	return Super::ProcessAutoLogin();
}

void AAuraGameSession::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();
}

void AAuraGameSession::HandleMatchHasEnded()
{
	Super::HandleMatchHasEnded();
}
