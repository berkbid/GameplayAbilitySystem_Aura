// Copyright Berkeley Bidwell

#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"

AAuraPlayerController::AAuraPlayerController()
{
	// @TODO: Does this need to be set to true??
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Set up input mapping context
	
	// Halts execution if this condition fails
	check(AuraContext);
	UEnhancedInputLocalPlayerSubsystem* InputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(InputSubsystem);
	InputSubsystem->AddMappingContext(AuraContext, 0);

	// Set cursor properties
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// Set up input mode
	FInputModeGameAndUI InputMode;
	InputMode.SetHideCursorDuringCapture(true);
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	SetInputMode(InputMode);
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
}
