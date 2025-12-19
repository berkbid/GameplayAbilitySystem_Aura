// Copyright Berkeley Bidwell

#pragma once

#include "AbilitySystem/Abilities/AuraDamageGameplayAbility.h"
#include "AuraGameplayAbility_Beam.generated.h"

class UObject;
class AActor;
class ACharacter;
class APlayerController;
struct FHitResult;

/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraGameplayAbility_Beam : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	AURA_API bool OnValidMouseData(const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable)
	AURA_API void StoreOwnerVariables();
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
};
