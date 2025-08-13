// Copyright Berkeley Bidwell

#pragma once

#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAuraWidgetController;
class UObject;
class UUserWidget;
class UAuraUserWidget;
class UHUDWidgetController;
class UAttributeMenuWidgetController;
class USpellMenuWidgetController;
struct FWidgetControllerParams;

/**
 * This class seems to only exist on local controller
 */
UCLASS(Config = Game)
class AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	AAuraHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void InitHUD(const FWidgetControllerParams& InControllerParams);
	
	UHUDWidgetController* GetHUDWidgetController(const FWidgetControllerParams& InControllerParams);
	
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& InControllerParams);

	USpellMenuWidgetController* GetSpellMenuWidgetController(const FWidgetControllerParams& InControllerParams);

	/** Toggles the escape menu on or off, returns true if escape menu is active and false if not */
	bool ToggleEscapeMenu();
	
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category="UI")
	TObjectPtr<UAuraUserWidget> AuraHUD;

private:
	template <typename ControllerT = UAuraWidgetController>
	ControllerT* GetOrCreateWidgetController(TObjectPtr<ControllerT>& WC, const TSubclassOf<UAuraWidgetController>& WCClass, const FWidgetControllerParams& InControllerParams);

private:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UAuraUserWidget> HudClass;

	UPROPERTY(Transient)
	TObjectPtr<UHUDWidgetController> HUDWidgetController;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UHUDWidgetController> HUDWidgetControllerClass;
	
	UPROPERTY(Transient)
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY(Transient)
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;
	
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSoftClassPtr<UUserWidget> MenuClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> AuraMenu;
};

