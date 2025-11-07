// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Player/AuraPlayerState.h"
#include "Interaction/CombatInterface.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "System/AuraGameInstance.h"
#include "UI/HUD/AuraHUD.h"
#include "UI/WidgetController/AuraWidgetController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAbilitySystemLibrary)

class UAttributeSet;

bool UAuraAbilitySystemLibrary::MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutParams, AAuraHUD*& OutAuraHUD)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (APlayerController* PlayerController = World ? World->GetFirstPlayerController() : nullptr)
	{
		if (AAuraPlayerState* PlayerState = PlayerController->GetPlayerState<AAuraPlayerState>())
		{
			UAbilitySystemComponent* ASC = PlayerState->GetAbilitySystemComponent();
			UAttributeSet* AttributeSet = PlayerState->GetAttributeSet();
			
			if (ASC && AttributeSet)
			{
				OutParams = FWidgetControllerParams(PlayerController, PlayerState, ASC, AttributeSet);
				OutAuraHUD = Cast<AAuraHUD>(PlayerController->GetHUD());
				return true;
			}
		}
	}
	return false;
}

UHUDWidgetController* UAuraAbilitySystemLibrary::GetHUDWidgetController(const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD = nullptr;
	FWidgetControllerParams Params;
	if (MakeWidgetControllerParams(WorldContextObject, Params, AuraHUD) && AuraHUD)
	{
		return AuraHUD->GetHUDWidgetController(Params);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Failed to get hud widget controller"));
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD = nullptr;
	FWidgetControllerParams Params;
	if (MakeWidgetControllerParams(WorldContextObject, Params, AuraHUD) && AuraHUD)
	{
		return AuraHUD->GetAttributeMenuWidgetController(Params);
	}

	UE_LOG(LogTemp, Warning, TEXT("Failed to get attribute menu widget controller"));
	return nullptr;
}

USpellMenuWidgetController* UAuraAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	AAuraHUD* AuraHUD = nullptr;
	FWidgetControllerParams Params;
	if (MakeWidgetControllerParams(WorldContextObject, Params, AuraHUD) && AuraHUD)
	{
		return AuraHUD->GetSpellMenuWidgetController(Params);
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Failed to get spell menu widget controller"));
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

void UAuraAbilitySystemLibrary::GiveCommonStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	checkf(ASC, TEXT("Failed to initialize default attributes, null ASC."));
	
	if (UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		// Grant common abilities for all classes
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : ClassInfo->CommonAbilities)
		{
			// Create ability spec from class
			const FGameplayAbilitySpec AbilitySpec(AbilityClass, 1);
			// Give the ability to the ASC
			ASC->GiveAbility(AbilitySpec);
		}

		// Get player level to give the ability of the correct level (if spawning character with higher level than 1)
		int32 PlayerLevel = 1;
		const AActor* AvatarActor = ASC->GetAvatarActor();
		if (AvatarActor && AvatarActor->Implements<UCombatInterface>())
		{
			PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(AvatarActor);
		}
		
		// Grant startup abilities for specific character class
		const FCharacterClassDefaultInfo& ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
		for (const TSubclassOf<UGameplayAbility>& AbilityClass : ClassDefaultInfo.StartupAbilities)
		{
			// Create ability spec from class
			const FGameplayAbilitySpec AbilitySpec(AbilityClass, PlayerLevel);
			// Give the ability to the ASC
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UAuraAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	const UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	return AuraGameInstance ? AuraGameInstance->CharacterClassInfo : nullptr;
}

UAbilityInfo* UAuraAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	const UAuraGameInstance* AuraGameInstance = Cast<UAuraGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
	return AuraGameInstance ? AuraGameInstance->AbilityInfo : nullptr;
}

bool UAuraAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->IsBlockedHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->IsCriticalHit();
	}
	return false;
}

bool UAuraAbilitySystemLibrary::IsDebuffSuccessful(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->IsDebuffSuccessful();
	}
	return false;
}

float UAuraAbilitySystemLibrary::GetDebuffDamage(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->GetDebuffDamage();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffFrequency(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->GetDebuffFrequency();
	}
	return 0.f;
}

float UAuraAbilitySystemLibrary::GetDebuffDuration(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->GetDebuffDuration();
	}
	return 0.f;
}

FGameplayTag UAuraAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->GetDamageType();
	}
	return FGameplayTag();
}

FVector UAuraAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

FVector UAuraAbilitySystemLibrary::GetKnockback(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* GameplayEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		return GameplayEffectContext->GetKnockback();
	}
	return FVector::ZeroVector;
}

void UAuraAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& ContextHandle, bool bInIsBlocked)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetIsBlockedHit(bInIsBlocked);
	}
}

void UAuraAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& ContextHandle, bool bInIsCritical)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetIsCriticalHit(bInIsCritical);
	}
}

void UAuraAbilitySystemLibrary::SetIsDebuffSuccessful(FGameplayEffectContextHandle& ContextHandle, bool bInIsDebuffSuccessful)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetIsDebuffSuccessful(bInIsDebuffSuccessful);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDamage(FGameplayEffectContextHandle& ContextHandle, float InDebuffDamage)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetDebuffDamage(InDebuffDamage);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffFrequency(FGameplayEffectContextHandle& ContextHandle, float InDebuffFrequency)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetDebuffFrequency(InDebuffFrequency);
	}
}

void UAuraAbilitySystemLibrary::SetDebuffDuration(FGameplayEffectContextHandle& ContextHandle, float InDebuffDuration)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetDebuffDuration(InDebuffDuration);
	}
}

void UAuraAbilitySystemLibrary::SetDamageType(FGameplayEffectContextHandle& ContextHandle, const FGameplayTag& InDamageType)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetDamageType(InDamageType);
	}
}

void UAuraAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& ContextHandle, const FVector& InDeathImpulse)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetDeathImpulse(InDeathImpulse);
	}
}

void UAuraAbilitySystemLibrary::SetKnockback(FGameplayEffectContextHandle& ContextHandle, const FVector& InKnockback)
{
	if (FAuraGameplayEffectContext* GameplayEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		GameplayEffectContext->SetKnockback(InKnockback);
	}
}

TArray<FVector> UAuraAbilitySystemLibrary::GetEffectContextSpawnLocations(const FGameplayEffectContextHandle& ContextHandle)
{
	if (const FAuraGameplayEffectContext* AuraEffectContext = static_cast<const FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		const TArray<FVector_NetQuantize>& Locations = AuraEffectContext->GetSpawnLocations();
		
		TArray<FVector> TempSpawnLocations;
		for (const FVector_NetQuantize& SpawnLocation : Locations)
		{
			TempSpawnLocations.Add(SpawnLocation);
		}
		return TempSpawnLocations;
	}
	return TArray<FVector>();
}

void UAuraAbilitySystemLibrary::SetEffectContextSpawnLocations(FGameplayEffectContextHandle& ContextHandle, const TArray<FVector>& InSpawnLocations)
{
	if (FAuraGameplayEffectContext* AuraEffectContext = static_cast<FAuraGameplayEffectContext*>(ContextHandle.Get()))
	{
		TArray<FVector_NetQuantize> TempSpawnLocations;
		for (const FVector& SpawnLocation : InSpawnLocations)
		{
			TempSpawnLocations.Add(SpawnLocation);
		}
		AuraEffectContext->SetSpawnLocations(TempSpawnLocations);
	}
}

FGameplayEffectContextHandle UAuraAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageParams)
{
	check(DamageParams.SourceASC);
	check(DamageParams.TargetASC);
	
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		
	// Make the GE spec handle
	FGameplayEffectContextHandle EffectContextHandle = DamageParams.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(DamageParams.SourceASC->GetAvatarActor());
	
	// Set the death impulse
	SetDeathImpulse(EffectContextHandle, DamageParams.DeathImpulse);
	
	// Set the knockback
	SetKnockback(EffectContextHandle, DamageParams.Knockback);
	
	const FGameplayEffectSpecHandle SpecHandle = DamageParams.SourceASC->MakeOutgoingSpec(DamageParams.DamageGameplayEffectClass, DamageParams.AbilityLevel, EffectContextHandle);
	
	if (FGameplayEffectSpec* Spec = SpecHandle.Data.Get())
	{
		// Assign set by caller magnitudes
		Spec->SetSetByCallerMagnitude(DamageParams.DamageType, DamageParams.BaseDamage);
		
		Spec->SetSetByCallerMagnitude(AuraGameplayTags.Debuff_Info_Damage, DamageParams.DebuffDamage);
		Spec->SetSetByCallerMagnitude(AuraGameplayTags.Debuff_Info_Chance, DamageParams.DebuffChance);
		Spec->SetSetByCallerMagnitude(AuraGameplayTags.Debuff_Info_Frequency, DamageParams.DebuffFrequency);
		Spec->SetSetByCallerMagnitude(AuraGameplayTags.Debuff_Info_Duration, DamageParams.DebuffDuration);
	}
		
	// Apply the GE spec to target
	DamageParams.TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data);

	return EffectContextHandle;
}

void UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);

	// query scene to see what we hit
	TArray<FOverlapResult> Overlaps;
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
	}

	for (const FOverlapResult& Overlap : Overlaps)
	{
		AActor* const OverlapActor = Overlap.GetActor();
		
		// Add actor to out actor array if it is not dead
		if (IsValid(OverlapActor) && OverlapActor->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(OverlapActor))
		{
			OutOverlappingActors.AddUnique(OverlapActor);
		}
	}
}

bool UAuraAbilitySystemLibrary::IsNotFriend(const AActor* FirstActor, const AActor* SecondActor)
{
	if (!FirstActor || !SecondActor)
	{
		// Default return the actors are friends
		return false;
	}
	
	if (FirstActor->ActorHasTag(FName("Player")))
	{
		return SecondActor->ActorHasTag(FName("Enemy"));
	}
	
	if (FirstActor->ActorHasTag(FName("Enemy")))
	{
		return SecondActor->ActorHasTag(FName("Player"));
	}

	// If the first or second actor doesn't have either tag, or they have the same tag, return false meaning they are friends
	return false;
}

bool UAuraAbilitySystemLibrary::AreFriends(const AActor* FirstActor, const AActor* SecondActor)
{
	if (!FirstActor || !SecondActor)
	{
		return false;
	}
	
	if (FirstActor->ActorHasTag(FName("Player")))
	{
		return SecondActor->ActorHasTag(FName("Player"));
	}
	
	if (FirstActor->ActorHasTag(FName("Enemy")))
	{
		return SecondActor->ActorHasTag(FName("Enemy"));
	}

	// If the first or second actor doesn't have either tag, or they have the same tag, return false meaning they are friends
	return false;
}

int32 UAuraAbilitySystemLibrary::GetXpRewardFromEnemy(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel)
{
	if (UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject))
	{
		const FCharacterClassDefaultInfo& ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
		const float XpReward = ClassDefaultInfo.XpReward.GetValueAtLevel(CharacterLevel);
		// Could also static cast to int32 instead of round to int
		return FMath::RoundToInt(XpReward);
	}
	return 0;
}
