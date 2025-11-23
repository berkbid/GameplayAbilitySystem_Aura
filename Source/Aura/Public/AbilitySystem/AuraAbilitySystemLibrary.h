// Copyright Berkeley Bidwell

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"
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
struct FDamageEffectParams;

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
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API bool IsDebuffSuccessful(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API float GetDebuffDamage(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API float GetDebuffFrequency(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API float GetDebuffDuration(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API FGameplayTag GetDamageType(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API FVector GetDeathImpulse(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API FVector GetKnockback(const FGameplayEffectContextHandle& ContextHandle);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool bInIsBlocked);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool bInIsCritical);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetIsDebuffSuccessful(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, bool bInIsDebuffSuccessful);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetDebuffDamage(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, float InDebuffDamage);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetDebuffFrequency(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, float InDebuffFrequency);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetDebuffDuration(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, float InDebuffDuration);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetDamageType(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const FGameplayTag& InDamageType);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetDeathImpulse(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const FVector& InDeathImpulse);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetKnockback(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const FVector& InKnockback);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API TArray<FVector> GetEffectContextSpawnLocations(const FGameplayEffectContextHandle& ContextHandle);

	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayEffects")
	static AURA_API void SetEffectContextSpawnLocations(UPARAM(ref) FGameplayEffectContextHandle& ContextHandle, const TArray<FVector>& InSpawnLocations);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|DamageEffects")
	static AURA_API FGameplayEffectContextHandle ApplyDamageEffect(const FDamageEffectParams& DamageParams);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& Origin);

	/** Returns true if both actors have tag of either player or enemy, and they are opposite of each other */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API bool IsNotFriend(const AActor* FirstActor, const AActor* SecondActor);

	/** Returns true if both actors have the same tag of either player or enemy */
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API bool AreFriends(const AActor* FirstActor, const AActor* SecondActor);

	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API TArray<FRotator> EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API TArray<FVector> EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|GameplayMechanics")
	static AURA_API int32 GetXpRewardFromEnemy(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);
};
