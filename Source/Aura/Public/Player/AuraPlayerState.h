// Copyright Berkeley Bidwell

#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/ModifierDependencyInterface.h"
#include "AuraPlayerState.generated.h"

class AAuraPlayerController;
class ULevelUpInfo;
class UAbilitySystemComponent;
class UAuraAbilitySystemComponent;
class UAttributeSet;
class UObject;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStatsUpdate, int32 /*StatValue*/);

/**
 * 
 */
UCLASS(MinimalAPI)
class AAuraPlayerState : public APlayerState, public IAbilitySystemInterface, public ICombatInterface, public IModifierDependencyInterface
{
	GENERATED_BODY()

public:
	AURA_API AAuraPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	AURA_API virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category = "Aura|PlayerState")
	AURA_API AAuraPlayerController* GetAuraPlayerController() const;
	
	UFUNCTION(BlueprintCallable, Category = "Aura|PlayerState")
	UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const { return AbilitySystemComponent; }
	
	// ~ IAbilitySystemInterface
	AURA_API virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	// ~ End IAbilitySystemInterface

	UFUNCTION(BlueprintPure)
	UAttributeSet* GetAttributeSet() const { return AttributeSet; }
	
	// ~ ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() const override { return Level; }
	virtual FOnDeath& GetOnDeathDelegate() override { return OnDeath; }
	// ~ End ICombatInterface
	
	/** Server call to add to Xp */
	UFUNCTION(BlueprintCallable)
	AURA_API void AddXp(int32 XpToAdd);
	
	UFUNCTION(BlueprintPure)
	int32 GetXp() const { return Xp; }
	
	/** Server call to add to Xp */
	UFUNCTION(BlueprintCallable)
	AURA_API bool AddOrRefundAttributePoints(const FGameplayTag& AttributeTag, int32 AttributePointsToAdd);
	
	UFUNCTION(BlueprintPure)
	int32 GetAttributePoints() const { return AttributePoints; }
	
	/** Server call to add to Xp */
	UFUNCTION(BlueprintCallable)
	AURA_API bool SpendOrRefundSpellPoints(const FGameplayTag& AbilityTag, int32 SpellPointsToAdd);
	
	UFUNCTION(BlueprintPure)
	int32 GetSpellPoints() const { return SpellPoints; }
	
public:
	FOnPlayerStatsUpdate OnXpUpdate;
	FOnPlayerStatsUpdate OnLevelUpdate;
	FOnPlayerStatsUpdate OnAttributeUpdate;
	FOnPlayerStatsUpdate OnSpellPointUpdate;

	UPROPERTY(BlueprintAssignable, Category="Aura|Death")
	FOnDeath OnDeath;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo;
	
protected:
	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UAuraAbilitySystemComponent> AbilitySystemComponent;

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
	
	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints);
	
	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints);
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Level)
	int32 Level = 1;

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_Xp)
	int32 Xp = 0;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 0;
	
};
