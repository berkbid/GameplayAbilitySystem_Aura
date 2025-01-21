// Copyright Berkeley Bidwell

#pragma once

#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class AActor;
class AController;
class UCameraComponent;
class USpringArmComponent;
class UObject;

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// ~ Begin APawn Interface.
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	// ~ End APawn Interface
	
	// ~ Begin ICombatInterface
	virtual int32 GetPlayerLevel() override;
	// UFUNCTION(BlueprintImplementableEvent)
	// void SetFacingTarget(const FVector& FacingTarget) override;
	// ~ End ICombatInterface
	
protected:
	/** Initializes the ability system component with owner actor/avatar */
	virtual void InitAbilityActorInfo() override;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	
};
