// Copyright Berkeley Bidwell

#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "AuraPlayerState.generated.h"

class ULevelUpInfo;
class UAbilitySystemComponent;
class UAttributeSet;
class UObject;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatsUpdate, int32 /*StatValue*/);

/**
 * 
 */
UCLASS(MinimalAPI)
class AAuraPlayerState : public APlayerState, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	AURA_API AAuraPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AURA_API virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// ~ Begin IAbilitySystemInterface
	AURA_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	// ~ End IAbilitySystemInterface

	UFUNCTION(BlueprintPure)
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	// ~ Begin ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() const override { return Level; }
	// ~ End ICombatInterface

	/** Server call to add to Xp */
	UFUNCTION(BlueprintCallable)
	AURA_API void AddXp(int32 XpToAdd);
	
	UFUNCTION(BlueprintPure)
	int32 GetXp() const { return Xp; }

public:
	FOnPlayerStatsUpdate OnXpUpdate;
	FOnPlayerStatsUpdate OnLevelUpdate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
	TObjectPtr<UAttributeSet> AttributeSet;

private:
	/** Cannot call directly, must be called from AddXp */
	UFUNCTION(BlueprintCallable)
	void SetLevel(int32 InLevel);
	
	UFUNCTION()
	void OnRep_Level(int32 OldLevel);
	
	UFUNCTION()
	void OnRep_Xp(int32 OldXp);
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Xp)
	int32 Xp = 0;
};
