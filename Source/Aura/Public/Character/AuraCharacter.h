// Copyright Berkeley Bidwell

#pragma once

#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class AActor;
class AController;
class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UObject;

USTRUCT(BlueprintType)
struct FCameraOccludedMeshActor
{
	GENERATED_BODY()

	// Assuming just 1 static mesh component coming from a static mesh actor
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TArray<UMaterialInterface*> Materials;
};

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
	virtual int32 GetPlayerLevel_Implementation() const override;
	// UFUNCTION(BlueprintImplementableEvent)
	// void SetFacingTarget(const FVector& FacingTarget) override;
	// ~ End ICombatInterface
	
protected:
	/** Initializes the ability system component with owner actor/avatar */
	virtual void InitAbilityActorInfo() override;
	
	virtual void InitializeDefaultAttributes() const override;
	
	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditAnywhere, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	
	// Could potentially move these 3 gameplay effects to the base class if we wanted enemies to utilize them
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributesClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributesClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributesClass;

	UPROPERTY(Transient,VisibleAnywhere, BlueprintReadOnly, Category="OccludedMeshes")
	TMap<AActor*, FCameraOccludedMeshActor> OccludedMeshActors;
};
