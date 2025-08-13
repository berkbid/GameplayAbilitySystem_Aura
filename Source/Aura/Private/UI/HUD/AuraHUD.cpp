// Copyright Berkeley Bidwell

#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "UI/WidgetController/HUDWidgetController.h"
#include "UI/WidgetController/SpellMenuWidgetController.h"

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
		
		// Broadcast initial values directly after widget controller is set on all sub-widgets on HUD
		HUDWidgetController->BroadcastInitialValues();
		
		AuraHUD->AddToViewport();
	}
}

UHUDWidgetController* AAuraHUD::GetHUDWidgetController(const FWidgetControllerParams& InControllerParams)
{
	return GetOrCreateWidgetController(HUDWidgetController, HUDWidgetControllerClass, InControllerParams);
}

UAttributeMenuWidgetController* AAuraHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& InControllerParams)
{
	return GetOrCreateWidgetController(AttributeMenuWidgetController, AttributeMenuWidgetControllerClass, InControllerParams);
}

USpellMenuWidgetController* AAuraHUD::GetSpellMenuWidgetController(const FWidgetControllerParams& InControllerParams)
{
	return GetOrCreateWidgetController(SpellMenuWidgetController, SpellMenuWidgetControllerClass, InControllerParams);
}

template <typename ControllerT>
ControllerT* AAuraHUD::GetOrCreateWidgetController(TObjectPtr<ControllerT>& WC, const TSubclassOf<UAuraWidgetController>& WCClass, const FWidgetControllerParams& InControllerParams)
{
	if (!WC)
	{
		checkf(WCClass, TEXT("%s uninitialized, please fill out B_AuraHUD"), *ControllerT::StaticClass()->GetName());
		WC = NewObject<ControllerT>(this, WCClass);
		WC->SetWidgetControllerParams(InControllerParams);
	}
	return WC;
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