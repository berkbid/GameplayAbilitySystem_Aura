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
UCLASS()
class UAuraGameplayAbility_Beam : public UAuraDamageGameplayAbility
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	bool OnValidMouseData(const FHitResult& Hit);
	
	UFUNCTION(BlueprintCallable)
	void StoreOwnerVariables();
	
	UFUNCTION(BlueprintCallable)
	void TraceFirstTarget(const FVector& BeamTargetLocation);
	
	UFUNCTION(BlueprintCallable)
	void StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets);
	
	UFUNCTION(BlueprintImplementableEvent)
	void PrimaryTargetDied(AActor* DeadActor);
	
	UFUNCTION(BlueprintImplementableEvent)
	void AdditionalTargetDied(AActor* DeadActor);
	
protected:
	/** Call to unbind from all targets their ondeath event, and null them out. */
	UFUNCTION(BlueprintCallable)
	void ClearAllTargets();
	
protected:
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	FVector MouseHitLocation;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<AActor> MouseHitActor;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TMap<AActor*, FGameplayCueParameters> AdditionalTargetsMap;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<APlayerController> OwnerPlayerController;
	
	UPROPERTY(BlueprintReadWrite, Category = "Beam")
	TObjectPtr<ACharacter> OwnerCharacter;
	
	UPROPERTY(EditDefaultsOnly, Category = "Beam")
	int32 MaxNumShockTargets = 5;
};
