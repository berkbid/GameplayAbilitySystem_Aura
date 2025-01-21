// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Player/AuraPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAbilitySystemLibrary)

class UAttributeSet;

UHUDWidgetController* UAuraAbilitySystemLibrary::GetHUDWidgetController(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr)
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			if (AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>())
			{
				UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
				UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
				
				if (ASC && AttributeSet)
				{
					return AuraHUD->GetHUDWidgetController(FWidgetControllerParams(PlayerController, PlayerState, ASC, AttributeSet));	
				}
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Failed to get hud widget controller"));
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);

	if (APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr)
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD()))
		{
			if (AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>())
			{
				UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
				UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
				
				if (ASC && AttributeSet)
				{
					return AuraHUD->GetAttributeMenuWidgetController(FWidgetControllerParams(PlayerController, PlayerState, ASC, AttributeSet));	
				}
			}
		}
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Failed to get attribute menu widget controller"));
	return nullptr;
}
