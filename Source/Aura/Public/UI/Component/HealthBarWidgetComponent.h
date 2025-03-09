// Copyright Berkeley Bidwell

#pragma once

#include "Components/WidgetComponent.h"
#include "HealthBarWidgetComponent.generated.h"

class UObject;

/**
 * 
 */
UCLASS()
class AURA_API UHealthBarWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UHealthBarWidgetComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	virtual void InitWidget() override;
	
	
};
