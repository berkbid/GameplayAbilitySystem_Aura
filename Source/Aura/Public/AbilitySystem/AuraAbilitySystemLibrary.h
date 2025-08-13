// Copyright Berkeley Bidwell

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "AuraAbilitySystemLibrary.generated.h"

class AAuraHUD;
class UCharacterClassInfo;
class UAbilityInfo;
enum class ECharacterClass : uint8;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
class UHUDWidgetController;
class UAbilitySystemComponent;
class UObject;
struct FGameplayEffectContextHandle;
struct FWidgetControllerParams;

/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/** Returns true if valid widget controller params, must still check OutAuraHUD pointer validity */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject", AutoCreateRefTerm = "OutParams"))
	static AURA_API bool MakeWidgetControllerParams(const UObject* WorldContextObject, FWidgetControllerParams& OutParams, AAuraHUD*& OutAuraHUD);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static AURA_API UHUDWidgetController* GetHUDWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static AURA_API UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController", meta = (DefaultToSelf = "WorldContextObject"))
	static AURA_API USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static AURA_API void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float CharacterLevel, UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static AURA_API void GiveCommonStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static AURA_API UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|AbilityInfo")
	static AURA_API UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API bool IsBlockedHit(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API bool IsCriticalHit(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool bInIsBlocked);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool bInIsCritical);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API TArray<FVector> GetEffectContextSpawnLocations(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetEffectContextSpawnLocations(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const TArray<FVector>& InSpawnLocations);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin);

	/** Returns true if both actors have tag of either player or enemy, and they are opposite of each other */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);

	/** Returns true if both actors have the same tag of either player or enemy */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API bool AreFriends(const AActor* FirstActor, const AActor* SecondActor);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API int32 GetXpRewardFromEnemy(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);
};
