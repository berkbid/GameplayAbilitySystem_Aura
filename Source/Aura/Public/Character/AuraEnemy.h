// Copyright Berkeley Bidwell

#pragma once

#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

class UEnemyWidgetController;
class AActor;
class AController;
class UObject;
class UHealthBarWidgetComponent;
class UBehaviorTree;
class AAuraAIController;
struct FGameplayTag;

/**
 * 
 */
UCLASS(MinimalAPI)
class AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AURA_API AAuraEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Aura|AI")
	AURA_API AAuraAIController* GetAuraAIController() const;
	
	// ~ Begin AActor Interface.
	AURA_API virtual void NotifyActorBeginCursorOver() override;
	AURA_API virtual void NotifyActorEndCursorOver() override;
	// ~ End AActor Interface

	// ~ Begin APawn Interface.
	AURA_API virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	// ~ Begin ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() const override { return Level; }
	AURA_API virtual void Die(const FVector& DeathImpulse) override;
	// ~ End ICombatInterface

	// ~ Begin IEnemyInterface
	AURA_API virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	AURA_API virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	AURA_API virtual AActor* GetCombatTarget_Implementation() const override;
	// ~ End IEnemyInterface
	
	// AAuraCharacterBase
	AURA_API virtual void MulticastHandleDeath(const FVector& DeathImpulse) override;
	// ~AAuraCharacterBase
	
	UFUNCTION(BlueprintPure, Category ="UI")
	AURA_API UEnemyWidgetController* GetEnemyWidgetController();

public:
	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

	// TODO: Perhaps hold soft object ptr
	/** Current combat target set from behavior tree on server */
	UPROPERTY(Transient, BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;
	
protected:
	// ~ Begin AActor Interface.
	AURA_API virtual void BeginPlay() override;
	// ~ End AActor Interface
	
	// ~ Begin UAuraCharacterBase
	AURA_API virtual void InitAbilityActorInfo() override;
	AURA_API virtual void AddCharacterAbilities() const override;
	// Override to initialize default attributes using aura ability system library
	AURA_API virtual void InitializeDefaultAttributes() const override;
	AURA_API virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount) override;
	// ~ End UAuraCharacterBase
	
protected:
	// Only server needs to know level of AI enemies, so not replicated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ExposeOnSpawn = "true"), Category="Character Class Defaults")
	int32 Level = 1;
	
	/** Class for enemy widget controller */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UEnemyWidgetController> EnemyWidgetControllerClass;

	UPROPERTY(EditDefaultsOnly, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, Category = "Combat")
	FName LeftHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName RightHandSocketName;
	UPROPERTY(EditAnywhere, Category = "Combat")
	FName TailSocketName;
	
private:
	void OnHitReactTagCountChanged(const FGameplayTag GameplayTag, int32 TagCount);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthBarWidgetComponent> HealthBar;

	/** Widget controller for enemies, each client and server has one for each enemy */
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyWidgetController> EnemyWidgetController;
};
