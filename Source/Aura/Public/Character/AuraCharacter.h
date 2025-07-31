// Copyright Berkeley Bidwell

#pragma once

#include "Character/AuraCharacterBase.h"
#include "Interaction/PlayerInterface.h"
#include "AuraCharacter.generated.h"

class AActor;
class AController;
class UCameraComponent;
class USpringArmComponent;
class UStaticMeshComponent;
class UMaterialInterface;
class UNiagaraComponent;
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
UCLASS(MinimalAPI)
class AAuraCharacter : public AAuraCharacterBase, public IPlayerInterface
{
	GENERATED_BODY()

public:
	AURA_API AAuraCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	// ~ Begin APawn Interface.
	AURA_API virtual void PossessedBy(AController* NewController) override;
	AURA_API virtual void OnRep_PlayerState() override;
	// ~ End APawn Interface
	
	// ~ Begin ICombatInterface
	AURA_API virtual int32 GetPlayerLevel_Implementation() const override;
	// UFUNCTION(BlueprintImplementableEvent)
	// AURA_API void SetFacingTarget(const FVector& FacingTarget) override;
	// ~ End ICombatInterface

	// Player Interface
	AURA_API virtual void AddToXp_Implementation(int32 InXp) override;
	// ~End Player Interface
	
protected:
	AURA_API virtual void InitializeDefaultAttributes() const override;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> Camera;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UNiagaraComponent> LevelUpNiagaraComponent;
	
	// Could potentially move these 3 gameplay effects to the base class if we wanted enemies to utilize them
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributesClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributesClass;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributesClass;

	UPROPERTY(Transient,VisibleAnywhere, BlueprintReadOnly, Category="OccludedMeshes")
	TMap<AActor*, FCameraOccludedMeshActor> OccludedMeshActors;
	
private:
	virtual void InitAbilityActorInfo() override;

	/** Called when a player levels up */
	void LevelUpParticles() const;
	
};
