// Copyright Berkeley Bidwell

#include "UI/WidgetController/AuraWidgetController.h"

void UAuraWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& InWidgetControllerParams)
{
	WidgetControllerParams = InWidgetControllerParams;
	BindCallBacksToDependencies(); // Set up binds after we get references
}
