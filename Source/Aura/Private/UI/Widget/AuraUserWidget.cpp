// Copyright Berkeley Bidwell

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AuraWidgetController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraUserWidget)

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	check(InWidgetController);
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
