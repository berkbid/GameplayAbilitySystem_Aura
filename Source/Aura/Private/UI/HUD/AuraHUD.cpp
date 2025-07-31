// Copyright Berkeley Bidwell

#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/HUDWidgetController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraHUD)

AAuraHUD::AAuraHUD(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bStartWithTickEnabled = false;
}

void AAuraHUD::InitHUD(const FWidgetControllerParams& InControllerParams)
{
	checkf(HudClass, TEXT("HUD widget class uninitialized, please fill out B_AuraHUD"));
	
	// Looks like this class only exists on local controller
	if (!AuraHUD)
	{
		AuraHUD = CreateWidget<UAuraUserWidget>(GetWorld(), HudClass);
		// Must propagate to set widget controller calls on sub widgets such that they can bind to callbacks before broadcast
		// initial values happens afterwards
		AuraHUD->SetWidgetController(GetHUDWidgetController(InControllerParams));
		
		// Broadcast initial values directly after widget controller is set on all sub-widgets
		HUDWidgetController->BroadcastInitialValues();
		
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

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& InControllerParams)
{
	checkf(AttributeMenuWidgetControllerClass, TEXT("AttributeMenuWidgetControllerClass uninitialized, please fill out B_AuraHUD"));
	
	if (!AttributeMenuWidgetController)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(InControllerParams);
	}
	return AttributeMenuWidgetController;
}

bool AAuraHUD::ToggleEscapeMenu()
{
	if (AuraMenu)
	{
		AuraMenu->RemoveFromParent();
		AuraMenu = nullptr;
		return false;
	}

	if (ensure(!MenuClass.IsNull()))
	{
		if (UClass* MenuClassPtr = MenuClass.LoadSynchronous())
		{
			AuraMenu = CreateWidget<UUserWidget>(GetWorld(), MenuClassPtr);
			AuraMenu->AddToViewport();
			return true;
		}
	}
	
	return false;
}
