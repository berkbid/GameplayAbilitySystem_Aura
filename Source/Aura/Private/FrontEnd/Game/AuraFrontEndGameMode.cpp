// Copyright Berkeley Bidwell

#include "FrontEnd/Game/AuraFrontEndGameMode.h"
#include "FrontEnd/Player/AuraFrontEndPlayerController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraFrontEndGameMode)

AAuraFrontEndGameMode::AAuraFrontEndGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	//GameStateClass = AAuraGameStateBase::StaticClass();
	//GameSessionClass = AAuraGameSession::StaticClass();
	PlayerControllerClass = AAuraFrontEndPlayerController::StaticClass();
	//PlayerStateClass = AAuraPlayerState::StaticClass();
	//DefaultPawnClass = AAuraCharacter::StaticClass();
	//HUDClass = AAuraHUD::StaticClass();
}
