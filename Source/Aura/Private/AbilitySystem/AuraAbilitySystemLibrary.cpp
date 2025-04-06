// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Player/AuraPlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Game/AuraGameModeBase.h"
#include "Kismet/GameplayStatics.h"
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

void UAuraAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float CharacterLevel, UAbilitySystemComponent* ASC)
{
	checkf(ASC, TEXT("Failed to initialize default attributes, null ASC."));

	if (UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		AActor* AvatarActor = ASC->GetAvatarActor();

		// Set the source object on the effect context, using same effect context for each gameplay effect. Course creates separate ones.
		FGameplayEffectContextHandle EffectContextHandle = ASC->MakeEffectContext();
		EffectContextHandle.AddSourceObject(AvatarActor);
		
		// Get class default info for primary attributes
		const FCharacterClassDefaultInfo& ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);

		// Apply primary attributes
		const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, CharacterLevel, EffectContextHandle);
		PrimaryAttributesSpecHandle.Data.Get()->GetContext().AddSourceObject(AvatarActor);
		ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

		// Apply secondary attributes
		const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, CharacterLevel, EffectContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());

		// Apply vital attributes
		const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, CharacterLevel, EffectContextHandle);
		ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
	}
}

void UAuraAbilitySystemLibrary::GiveCommonStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	checkf(ASC, TEXT("Failed to initialize default attributes, null ASC."));
	
	if (UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : ClassInfo->CommonAbilities)
		{
			// Create ability spec from class
			const FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
			// Give the ability to the ASC
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AAuraGameModeBase* AuraGameMode = Cast<AAuraGameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	return AuraGameMode ? AuraGameMode->CharacterClassInfo : nullptr;
}
