// Copyright Berkeley Bidwell

#pragma once

#include "AuraWidgetController.generated.h"

class APlayerController;
class APlayerState;
class UAbilitySystemComponent;
class UAttributeSet;
class UObject;
class UGameplayAbility;
class UAbilityInfo;
struct FAuraAbilityInfo;

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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature, float, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStatChangedSignature, int32, NewValue);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityInfoSignature, const FAuraAbilityInfo&, AbilityInfo);


/**
 * UAuraWidgetController
 *
 * Base class for all Widget Controllers.
 * Responsible for getting data and broadcasting for widgets to listen to.
 */
UCLASS(MinimalAPI, Abstract, BlueprintType, Blueprintable)
class UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	/** Set the controller params and then this binds to callbacks */
	UFUNCTION(BlueprintCallable, Category="WidgetController")
	AURA_API void SetWidgetControllerParams(const FWidgetControllerParams& InWidgetControllerParams);

	virtual void BindCallBacksToDependencies() {};
	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues() {};
	
	/** Broadcast the ability info delegate for each activatable ability */
	AURA_API void BroadcastAbilityInfo() const;

protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	FWidgetControllerParams WidgetControllerParams;
	
	/** Broadcast for each activatable ability with its info, when they are given to ASC */
	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FAbilityInfoSignature AbilityInfoDelegate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Data")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
};
