// Copyright Berkeley Bidwell

#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/HUDWidgetController.h"

void AAuraHUD::InitHUD(const FWidgetControllerParams& InControllerParams)
{
	checkf(HudClass, TEXT("HUD widget class uninitialized, please fill out B_AuraHUD"));
	
	// Looks like this class only exists on local controller
	if (!AuraHUD)
	{
		AuraHUD = CreateWidget<UAuraUserWidget>(GetWorld(), HudClass);
		AuraHUD->SetWidgetController(GetHUDWidgetController(InControllerParams));
		AuraHUD->AddToViewport();
	}
}

UHUDWidgetController* AAuraHUD::GetHUDWidgetController(const FWidgetControllerParams& InControllerParams)
{
	checkf(HUDWidgetControllerClass, TEXT("HUDWidgetControllerClass uninitialized, please fill out B_AuraHUD"));
	
	if (!HUDWidgetController)
	{
		HUDWidgetController = NewObject<UHUDWidgetController>(this, HUDWidgetControllerClass);
		HUDWidgetController->SetWidgetControllerParams(InControllerParams);
	}
	
	return HUDWidgetController;
}
