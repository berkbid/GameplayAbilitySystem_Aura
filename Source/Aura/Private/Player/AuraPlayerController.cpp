// Copyright Berkeley Bidwell

#include "Player/AuraPlayerController.h"
//#include "Character/AuraCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

AAuraPlayerController::AAuraPlayerController()
{
	// @TODO: Does this need to be set to true??
	bReplicates = true;
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	if (IsLocalController())
	{
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
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	check(MoveAction);
	
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::AuraMove);
}

void AAuraPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);
	
	//PrintLocalRole(TEXT("ClientSetHUD: "));

	// Could check if playerstate exists instead
	if (HasAuthority())
	{
		InitHUD();
	}
}

void AAuraPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	InitHUD();
}

void AAuraPlayerController::InitHUD()
{
	AAuraHUD* AuraHUD = CastChecked<AAuraHUD>(GetHUD());
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);

	// Only issue with doing it here is the Ability Actor Info (owner/avatar) might not be set yet
	AuraHUD->InitHUD(FWidgetControllerParams(
		this, AuraPlayerState, AuraPlayerState->GetAbilitySystemComponent(), AuraPlayerState->GetAttributeSet()));
}

void AAuraPlayerController::AuraMove(const FInputActionValue& InputActionValue)
{
	if (APawn* ControlledPawn = GetPawn())
	{
		const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
		
		const FRotator YawRotation(0.f, GetControlRotation().Yaw, 0.f);
		
		// Get forward vector from this rotation
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::PrintLocalRole(const FString& InMessage) const
{
	UE_LOG(LogTemp, Warning, TEXT("%sLocal Role: %s"), *InMessage, *UEnum::GetValueAsString<ENetRole>(GetLocalRole()));
}
