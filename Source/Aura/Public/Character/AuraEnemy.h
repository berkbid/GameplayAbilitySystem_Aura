// Copyright Berkeley Bidwell

#pragma once

#include "Character/AuraCharacterBase.h"
#include "Interaction/EnemyInterface.h"
#include "AuraEnemy.generated.h"

class AActor;
class UObject;

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
	
protected:
	// ~ Begin AActor Interface.
	virtual void BeginPlay() override;
	// ~ End AActor Interface
	
	// Only server needs to know level of AI enemies, so not replicated
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Character Class Defaults")
	int32 Level = 1;

};
