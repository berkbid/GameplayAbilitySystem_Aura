// Copyright Berkeley Bidwell

#include "FrontEnd/Player/AuraFrontEndPlayerController.h"
#include "CommonSessionSubsystem.h"
#include "CommonUserSubsystem.h"
#include "GameFramework/GameModeBase.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraFrontEndPlayerController)

AAuraFrontEndPlayerController::AAuraFrontEndPlayerController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AAuraFrontEndPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraFrontEndPlayerController::ResetUserAndSession() const
{
	UE_LOG(LogTemp, Warning, TEXT("Resetting user and session"));
	bool bWasHardDisconnect = false;
	AGameModeBase* GameMode = GetWorld()->GetAuthGameMode<AGameModeBase>();
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);

	if (ensure(GameMode) && UGameplayStatics::HasOption(GameMode->OptionsString, TEXT("closed")))
	{
		bWasHardDisconnect = true;
	}

	// Only reset users on hard disconnect
	UCommonUserSubsystem* UserSubsystem = GameInstance->GetSubsystem<UCommonUserSubsystem>();
	if (ensure(UserSubsystem) && bWasHardDisconnect)
	{
		UserSubsystem->ResetUserState();
	} 
	
	// Always reset sessions
	UCommonSessionSubsystem* SessionSubsystem = GameInstance->GetSubsystem<UCommonSessionSubsystem>();
	if (ensure(SessionSubsystem))
	{
		SessionSubsystem->CleanUpSessions();
	}
}
