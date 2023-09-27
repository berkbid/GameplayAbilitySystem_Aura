// Copyright Berkeley Bidwell

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	check(InWidgetController);
	WidgetController = InWidgetController;
	WidgetControllerSet();
	
	// Broadcast after calling WidgetControllerSet() to allow blueprint children to bind to controller events
	UAuraWidgetController* AuraWidgetController = CastChecked<UAuraWidgetController>(WidgetController);
	AuraWidgetController->BroadcastInitialValues();
}
