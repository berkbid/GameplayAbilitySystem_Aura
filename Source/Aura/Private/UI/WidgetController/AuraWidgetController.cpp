// Copyright Berkeley Bidwell

#include "UI/WidgetController/AuraWidgetController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraWidgetController)

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& InWidgetControllerParams)
{
	// This happens one time before the HUD is added to viewport
	
	WidgetControllerParams = InWidgetControllerParams;
	BindCallBacksToDependencies(); // Set up binds after we get references
}
