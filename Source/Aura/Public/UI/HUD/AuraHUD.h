// Copyright Berkeley Bidwell

#pragma once

#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UObject;
class UUserWidget;
class UAuraUserWidget;
class UHUDWidgetController;
class UAttributeMenuWidgetController;
struct FWidgetControllerParams;

/**
 * This class seems to only exist on local controller
 */
UCLASS(Config = Game)
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	AAuraHUD(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void InitHUD(const FWidgetControllerParams& InControllerParams);
	
	UHUDWidgetController* GetHUDWidgetController(const FWidgetControllerParams& InControllerParams);
	
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& InControllerParams);

	/** Toggles the escape menu on or off, returns true if escape menu is active and false if not */
	bool ToggleEscapeMenu();
	
protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category="UI")
	TObjectPtr<UAuraUserWidget> AuraHUD;
	
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

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSoftClassPtr<UUserWidget> MenuClass;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> AuraMenu;
};
