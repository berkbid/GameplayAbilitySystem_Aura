// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AuraUserWidget.generated.h"

/**
 * Knows about AuraWidgetController
 */
UCLASS(Abstract)
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
