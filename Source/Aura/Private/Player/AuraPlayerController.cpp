// Copyright Berkeley Bidwell

#include "Player/AuraPlayerController.h"
#include "AuraGameplayTags.h"
#include "Player/AuraPlayerState.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

AAuraPlayerController::AAuraPlayerController()
{
	// @TODO: Does this need to be set to true??
	bReplicates = true;
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
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
		InputMode.SetHideCursorDuringCapture(false);
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

void AAuraPlayerController::Tick(float DeltaSeconds)
{
	// TODO: Should non-locally controlled PC even tick?
	
	Super::Tick(DeltaSeconds);
	
	AutoRun();
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning)
	{
		return;
	}

	APawn* ControlledPawn = GetPawn();
	if (IsValid(ControlledPawn)) // non-null and not pending kill
	{
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		
		// Could just use pawn location instead of location on spline
		//const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		const FVector Direction = Spline->FindTangentClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
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
	// Want to know if we are targeting an enemy when an ability is pressed, to determine if left click should move or use ability
	// TODO: Server is not setting or using these properties on autonomous proxy, do we care?
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		FHitResult CursorHit;
		GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
		
		// We are targeting an enemy if cursor hit is valid and the actor it his implements enemy interface
		bTargeting = CursorHit.IsValidBlockingHit() && Cast<IEnemyInterface>(CursorHit.GetActor());

		// Reset these values
		bAutoRunning = false;
		FollowTime = 0.f;
		bValidCachedDestination = false;
		
		// This is experimental code, why not get cached destination on initial press if we aren't targeting with LMB
		if (!bTargeting && CursorHit.IsValidBlockingHit())
		{
			CachedDestination = CursorHit.ImpactPoint;
			bValidCachedDestination = true;
		}
	}
}

void AAuraPlayerController::AbilityInputTagHeld(const FInputActionValue& InputActionValue, const FGameplayTag InputTag)
{
	// TODO: Server does not enforce targeting value, do we care?
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) || bTargeting)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		
		return;
	}

	// Tag == LMB AND we are not targeting: click to move behavior
	
	FollowTime += GetWorld()->GetDeltaSeconds();
	
	FHitResult Hit;
	if (GetHitResultUnderCursor(ECC_Visibility, false, Hit) && Hit.IsValidBlockingHit())
	{
		CachedDestination = Hit.ImpactPoint;
		bValidCachedDestination = true;
		
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FInputActionValue& InputActionValue, const FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB) || bTargeting)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}

	// Tag == LMB AND we are not targeting: click to move behavior

	if (FollowTime <= ShortPressThreshold && bValidCachedDestination)
	{
		if (const APawn* ControlledPawn = GetPawn())
		{
			// TODO: How do we know we have a valid cached destination?
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints();

				// TODO: Could use this instead of clearing spline points then adding each individually?
				//Spline->SetSplinePoints(NavPath->PathPoints, ESplineCoordinateSpace::World);
				
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
					//DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Blue, false, 5.f);
				}
				
				// Have auto run go to the last spline point instead of line trace location, as spline point is navigatable to through nav mesh
				//CachedDestination = Spline->GetSplinePointAt(Spline->GetNumberOfSplinePoints() - 1, ESplineCoordinateSpace::World).Position;
				const int32 LastPathPointIndex = NavPath->PathPoints.Num() - 1;
				if (NavPath->PathPoints.IsValidIndex(LastPathPointIndex))
				{
					CachedDestination = NavPath->PathPoints[LastPathPointIndex];
					// Only true if we have a valid cached destination
					bAutoRunning = true;
				}
			}
		}
	}
	
	FollowTime = 0.f;
	bTargeting = false;
	bValidCachedDestination = false;
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (!AuraAbilitySystemComponent)
	{
		const AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(AuraPlayerState ? AuraPlayerState->GetAbilitySystemComponent() : nullptr);
	}

	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::PrintLocalRole(const FString& InMessage) const
{
	UE_LOG(LogTemp, Warning, TEXT("%sLocal Role: %s"), *InMessage, *UEnum::GetValueAsString<ENetRole>(GetLocalRole()));
}
