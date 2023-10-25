// Copyright Berkeley Bidwell

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	check(InWidgetController);
	WidgetController = InWidgetController;
	WidgetControllerSet();
	
	// Broadcast after calling WidgetControllerSet() to allow blueprint children to bind to controller events
	// If more than one widget is connected to controller, each time widget controller is set on a widget, broadcast happens for
	// ALL widgets that have this widget controller bound already
	// TODO: Could take a bool input on this function if we should broadcast initial values, this would allow
	// multiple widgets to call setwidgetcontroller without having to broadcast every time
	UAuraWidgetController* AuraWidgetController = CastChecked<UAuraWidgetController>(WidgetController);
	AuraWidgetController->BroadcastInitialValues();
}
