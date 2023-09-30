// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraEnemy.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraEnemy : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraEnemy();

	// ~ Begin AActor Interface.
	virtual void NotifyActorBeginCursorOver() override;
	virtual void NotifyActorEndCursorOver() override;
	// ~ End AActor Interface
	
protected:
	// ~ Begin AActor Interface.
	virtual void BeginPlay() override;
	// ~ End AActor Interface
	
};
