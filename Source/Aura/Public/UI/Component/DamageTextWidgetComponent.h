// Copyright Berkeley Bidwell

#pragma once

#include "Components/WidgetComponent.h"
#include "DamageTextWidgetComponent.generated.h"

class UObject;

/**
 * 
 */
UCLASS()
class AURA_API UDamageTextWidgetComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetDamageText(float Damage, bool bBlockedHit, bool bCriticalHit);
	
};
