// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class IEnemyInterface;
struct FGameplayTag;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController();

protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void OnRep_PlayerState() override;
	/** Server can init hud here because pc and ps are valid */
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	
	void InitHUD();

	void PrintLocalRole(const FString& InMessage = "") const;

protected:
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
	
private:
	void AuraMove(const FInputActionValue& InputActionValue);
	void AbilityInputTagPressed(const FInputActionValue& InputActionValue, const FGameplayTag InputTag);
	void AbilityInputTagHeld(const FInputActionValue& InputActionValue, const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FInputActionValue& InputActionValue, const FGameplayTag InputTag);
	
	UAuraAbilitySystemComponent* GetASC();

	void AutoRun();
	
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY(Transient)
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;

	// Movement
	
	FVector CachedDestination = FVector::ZeroVector;
	bool bValidCachedDestination = false;
	float FollowTime = 0.f;
	float ShortPressThreshold = 0.5f;
	bool bAutoRunning = false;
	bool bTargeting = false;
	
	UPROPERTY(Transient, VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline;
	
};
