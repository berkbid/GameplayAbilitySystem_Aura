// Copyright Berkeley Bidwell

#pragma once

#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

class UEnemyWidgetController;
class AActor;
class UObject;
class UHealthBarWidgetComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	AAuraEnemy(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// ~ Begin AActor Interface.
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	// ~ End AActor Interface
	
	// ~ Begin ICombatInterface
	virtual int32 GetPlayerLevel() override { return Level; }
	// ~ End ICombatInterface

	UFUNCTION(BlueprintPure, Category ="UI")
	UEnemyWidgetController* GetEnemyWidgetController();
	
protected:
	// ~ Begin AActor Interface.
	virtual void BeginPlay() override;
	// ~ End AActor Interface
	
	virtual void InitAbilityActorInfo() override;

protected:
	// Only server needs to know level of AI enemies, so not replicated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

	/** Class for enemy widget controller */
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UEnemyWidgetController> EnemyWidgetControllerClass;
	
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthBarWidgetComponent> HealthBar;

	/** Widget controller for enemies, each client and server has one for each enemy */
	UPROPERTY(Transient, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEnemyWidgetController> EnemyWidgetController;
	
};
