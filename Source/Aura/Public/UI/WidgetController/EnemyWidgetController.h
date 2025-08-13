// Copyright Berkeley Bidwell

#pragma once

#include "UI/WidgetController/AuraWidgetController.h"
#include "EnemyWidgetController.generated.h"

class UObject;

/**
 * 
 */
UCLASS(MinimalAPI)
class UEnemyWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAuraWidgetController interface
	AURA_API virtual void BroadcastInitialValues() override;
	AURA_API virtual void BindCallBacksToDependencies() override;
	// ~ End UAuraWidgetController interface
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
};
