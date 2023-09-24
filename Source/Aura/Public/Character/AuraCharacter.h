// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraCharacter();

	// ~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// ~ End APawn Interface
	
protected:
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;

private:
	/** Initializes the ability system component with owner actor/avatar */
	void InitAbilityActorInfo();
	
};
