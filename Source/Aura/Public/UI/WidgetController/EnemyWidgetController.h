// Copyright Berkeley Bidwell

#pragma once

#include "UI/WidgetController/AuraWidgetController.h"
#include "EnemyWidgetController.generated.h"

class UObject;

/**
 * 
 */
UCLASS()
class AURA_API UEnemyWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAuraWidgetController interface
	virtual void BroadcastInitialValues() override;
	virtual void BindCallBacksToDependencies() override;
	// ~ End UAuraWidgetController interface
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
};
