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
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	UFUNCTION(BlueprintCallable, Category = "Aura|AI")
	AAuraAIController* GetAuraAIController() const;
	
	// ~ Begin AActor Interface.
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	// ~ End AActor Interface

	// ~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	// ~ End APawn Interface
	
	// ~ Begin ICombatInterface
	virtual int32 GetPlayerLevel_Implementation() const override { return Level; }
	virtual void Die() override;
	// ~ End ICombatInterface

	// ~ Begin IEnemyInterface
	virtual FVector GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const override;
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	// ~ End IEnemyInterface
	
	// AAuraCharacterBase
	virtual void MulticastHandleDeath() override;
	// ~AAuraCharacterBase
	
	UFUNCTION(BlueprintPure, Category ="UI")
	UEnemyWidgetController* GetEnemyWidgetController();

	UPROPERTY(BlueprintReadOnly, Category="Combat")
	bool bHitReacting = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 250.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float LifeSpan = 5.f;

	// TODO: Perhaps hold soft object ptr
	/** Current combat target set from behavior tree on server */
	UPROPERTY(Transient, BlueprintReadWrite, Category="Combat")
	TObjectPtr<AActor> CombatTarget;
	
protected:
	// ~ Begin AActor Interface.
	virtual void BeginPlay() override;
	// ~ End AActor Interface
	
	// ~ Begin UAuraCharacterBase
	virtual void InitAbilityActorInfo() override;
	virtual void AddCharacterAbilities() const override;
	// Override to initialize default attributes using aura ability system library
	virtual void InitializeDefaultAttributes() const override;
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
