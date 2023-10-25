// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAuraUserWidget;
class UHUDWidgetController;
class UAttributeMenuWidgetController;
struct FWidgetControllerParams;

/**
 * This class seems to only exist on local controller
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	void InitHUD(const FWidgetControllerParams& InControllerParams);
	
	UHUDWidgetController* GetHUDWidgetController(const FWidgetControllerParams& InControllerParams);
	
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& InControllerParams);

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
	
};
