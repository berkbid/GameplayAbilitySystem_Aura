// Copyright Berkeley Bidwell

#pragma once

#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

class UObject;

/**
 * Knows about AuraWidgetController
 */
UCLASS(Abstract, meta = (DisableNativeTick))
class AURA_API UAuraUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);
	
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	/** Blueprint can respond to when widget controller is set, should bind to controller events */
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
