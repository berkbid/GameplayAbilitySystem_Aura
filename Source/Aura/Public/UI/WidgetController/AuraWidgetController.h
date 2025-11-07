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
struct FGameplayTag;

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
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnAbilityStatusLevelChanged, const FGameplayTag&, AbilityTag, const FGameplayTag&, StatusTag, int32, Level);

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
	
	AURA_API void OnAbilityEquipped(const FGameplayTag& AbilityTag, const FGameplayTag& Status, const FGameplayTag& InputTag, const FGameplayTag& PrevInputTag) const;

protected:
	AURA_API UAbilityInfo* GetAbilityInfo() const;
	
	UFUNCTION(BlueprintCallable)
	AURA_API FAuraAbilityInfo GetAbilityInfoByAbilityTag(const FGameplayTag& AbilityTag) const;
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController")
	FWidgetControllerParams WidgetControllerParams;
	
	// Broadcast when a slot (input tag) has an ability equipped or unequipped from it
	UPROPERTY(BlueprintAssignable, Category="GAS|Ability")
	FAbilityInfoSignature OnSlotAbilityChanged;

	// Broadcast when an ability's status and/or level changes
	UPROPERTY(BlueprintAssignable, Category="GAS|Ability")
	FOnAbilityStatusLevelChanged OnAbilityStatusLevelChanged;
};
