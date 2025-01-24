// Copyright Berkeley Bidwell

#include "Game/AuraGameModeBase.h"
#include "Game/AuraGameStateBase.h"
#include "Character/AuraCharacter.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "System/AuraGameSession.h"
#include "UI/HUD/AuraHUD.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameModeBase)

AAuraGameModeBase::AAuraGameModeBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	GameStateClass = AAuraGameStateBase::StaticClass();
	GameSessionClass = AAuraGameSession::StaticClass();
	PlayerControllerClass = AAuraPlayerController::StaticClass();
	// ReplaySpectatorPlayerControllerClass = ALyraReplayPlayerController::StaticClass();
	PlayerStateClass = AAuraPlayerState::StaticClass();
	DefaultPawnClass = AAuraCharacter::StaticClass();
	HUDClass = AAuraHUD::StaticClass();
}
