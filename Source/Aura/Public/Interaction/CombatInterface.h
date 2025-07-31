// Copyright Berkeley Bidwell

#pragma once

#include "UObject/Interface.h"
#include "GameplayTagContainer.h"
#include "CombatInterface.generated.h"

enum class ECharacterClass : uint8;
class UNiagaraSystem;
class UObject;
class UAnimMontage;
class USoundBase;
class AActor;

USTRUCT(BlueprintType)
struct FTaggedMontage
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="Montage"))
	FGameplayTag MontageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="CombatSocket"))
	FGameplayTag SocketTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage = nullptr;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USoundBase> ImpactSound = nullptr;
};

UINTERFACE(MinimalAPI, BlueprintType)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ICombatInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	int32 GetPlayerLevel() const;
	virtual int32 GetPlayerLevel_Implementation() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FVector GetCombatSocketLocation(const FGameplayTag& MontageTag) const;
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetFacingTarget(const FVector& FacingTarget);

	virtual void Die();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UAnimMontage* GetHitReactMontage() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	bool IsDead() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	AActor* GetAvatar();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	TArray<FTaggedMontage> GetAttackMontages() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	UNiagaraSystem* GetBloodEffect();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTaggedMontage GetTaggedMontageByTag(const FGameplayTag& MontageTag) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetMinionCount() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IncrementMinionCount(int32 Amount = 1);
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	ECharacterClass GetCharacterClass() const;
};
