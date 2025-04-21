// Copyright Berkeley Bidwell

#pragma once

#include "CommonPlayerController.h"
#include "GameplayTagContainer.h"
#include "AuraPlayerController.generated.h"

class UDamageTextWidgetComponent;
class AAuraHUD;
class ACharacter;
class UObject;
class AHUD;
class IEnemyInterface;
struct FGameplayTag;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAuraInputConfig;
class UAuraAbilitySystemComponent;
class USplineComponent;

/**
 * AAuraPlayerController
 *
 *	The base player controller class used by this project.
 */
UCLASS()
class AURA_API AAuraPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:
	AAuraPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Aura|PlayerController")
	AAuraHUD* GetAuraHud() const;
	
	UFUNCTION(BlueprintCallable, Category = "Aura|PlayerController")
	UAuraAbilitySystemComponent* GetASC();

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);
	
protected:
	virtual void BeginPlay() override;
	
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;
	
	virtual void OnRep_PlayerState() override;
	/** Server can init hud here because pc and ps are valid */
	virtual void ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass) override;
	
	void InitHUD();
	
	UFUNCTION(BlueprintCallable, Category = "Aura|PlayerController")
	void ToggleEscapeMenu();
	
	void PrintLocalRole(const FString& InMessage = "") const;

protected:
	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;
private:
	void AuraMove(const FInputActionValue& InputActionValue);
	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	
	void AbilityInputTagPressed(const FInputActionValue& InputActionValue, const FGameplayTag InputTag);
	void AbilityInputTagHeld(const FInputActionValue& InputActionValue, const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FInputActionValue& InputActionValue, const FGameplayTag InputTag);
	
	void AutoRun();

	/** Update input mode for gameplay or for UI is parameter is false */
	void UpdateInputMode(bool bGameInputMode);
	
private:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> ShiftAction;
	
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MenuAction;
	
	/** Whether the shift key is currently down */
	bool bShiftKeyDown = false;
	
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

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<UDamageTextWidgetComponent> DamageTextComponentClass;
	
};
