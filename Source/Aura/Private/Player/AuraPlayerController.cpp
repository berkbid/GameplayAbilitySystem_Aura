// Copyright Berkeley Bidwell

#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Input/AuraInputComponent.h"
#include "Input/AuraInputConfig.h"
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

	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);
	check(MoveAction);
	
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::AuraMove);
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);
	
	PrintLocalRole(TEXT("ClientSetHUD: "));

	// Only autonomous proxy is in here, but authority has valid playerstate and doesn't get OnRep_PlayerState called
	// So if this is server's PC, init hud here
	// Could check if playerstate exists instead
	if (HasAuthority())
	{
		InitHUD();
	}
}

void AAuraPlayerController::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
	
	PrintLocalRole(TEXT("OnRep_PlayerState: "));
	
	InitHUD();
}

void AAuraPlayerController::InitHUD()
{
	PrintLocalRole(TEXT("Init HUD: "));

	if (IsLocalController())
	{
		AAuraHUD* AuraHUD = CastChecked<AAuraHUD>(GetHUD());
		AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
		check(AuraPlayerState);

		// Only issue with doing it here is the Ability Actor Info (owner/avatar) might not be set yet
		AuraHUD->InitHUD(FWidgetControllerParams(
			this, AuraPlayerState, AuraPlayerState->GetAbilitySystemComponent(), AuraPlayerState->GetAttributeSet()));
	}
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

void AAuraPlayerController::AbilityInputTagPressed(const FInputActionValue& InputActionValue, const FGameplayTag InputTag)
{
	//check(InputConfig);
	//const UInputAction* InputAction = InputConfig->FindAbilityInputActionForTag(InputTag);
	UE_LOG(LogTemp, Warning, TEXT("Pressed: %s"), *InputTag.ToString());
	
	// TODO: Use tag to activate ability if ability mapped to tag
}

void AAuraPlayerController::AbilityInputTagReleased(const FInputActionValue& InputActionValue, const FGameplayTag InputTag)
{
	//check(InputConfig);
	//const UInputAction* InputAction = InputConfig->FindAbilityInputActionForTag(InputTag);
	UE_LOG(LogTemp, Warning, TEXT("Released: %s"), *InputTag.ToString());
}

void AAuraPlayerController::AbilityInputTagHeld(const FInputActionValue& InputActionValue, const FGameplayTag InputTag)
{
	//check(InputConfig);
	//const UInputAction* InputAction = InputConfig->FindAbilityInputActionForTag(InputTag);
	UE_LOG(LogTemp, Warning, TEXT("Held: %s"), *InputTag.ToString());
}

void AAuraPlayerController::PrintLocalRole(const FString& InMessage) const
{
	UE_LOG(LogTemp, Warning, TEXT("%sLocal Role: %s"), *InMessage, *UEnum::GetValueAsString<ENetRole>(GetLocalRole()));
}
