// Copyright Berkeley Bidwell

#pragma once

#include "UI/WidgetController/AuraWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

struct FGameplayTag;
class UAttributeInfo;
class UObject;
struct FAuraAttributeInfo;
struct FGameplayAttribute;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FAuraAttributeInfo&, Info);

/**
 * 
 */
UCLASS(MinimalAPI)
class UAttributeMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()

public:
	// ~ Begin UAuraWidgetController interface
	AURA_API virtual void BroadcastInitialValues() override;
	AURA_API virtual void BindCallBacksToDependencies() override;
	// ~ End UAuraWidgetController interface

	UFUNCTION(BlueprintCallable)
	AURA_API void IncrementAttribute(const FGameplayTag& AttributeTag, int32 IncrementAmount);
	
public:
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnPlayerStatChangedSignature OnAttributePointsChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnPlayerStatChangedSignature OnSpellPointsChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

private:
	/** Gets the attribute info for a given attribute tag, and populates the value field with the gameplay attribute,
	 * then broadcasts the info */
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
