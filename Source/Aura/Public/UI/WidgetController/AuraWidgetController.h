// Copyright Berkeley Bidwell

#pragma once

#include "AuraWidgetController.generated.h"

class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;
class UObject;

USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<APlayerState> PlayerState = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet = nullptr;
	
	FWidgetControllerParams() {}
	
	FWidgetControllerParams(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
	: PlayerController(PC), PlayerState(PS), AbilitySystemComponent(ASC), AttributeSet(AS) {}
	
};

/**
 * UAuraWidgetController
 *
 * Base class for all Widget Controllers.
 * Responsible for getting data and broadcasting for widgets to listen to.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	/** Set the controller params and then this binds to callbacks */
	UFUNCTION(BlueprintCallable, Category="WidgetController")
	void SetWidgetControllerParams(const FWidgetControllerParams& InWidgetControllerParams);
	
	virtual void BroadcastInitialValues() {};
	virtual void BindCallBacksToDependencies() {};
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	FWidgetControllerParams WidgetControllerParams;
	
};
