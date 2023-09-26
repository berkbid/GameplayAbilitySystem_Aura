// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "AuraHUD.generated.h"

class UAuraUserWidget;
class UHUDWidgetController;
struct FWidgetControllerParams;

/**
 * This class seems to only exist on local controller
 */
UCLASS()
class AURA_API AAuraHUD : public AHUD
{
	GENERATED_BODY()

public:
	UHUDWidgetController* GetHUDWidgetController(const FWidgetControllerParams& InControllerParams);

	void InitHUD(const FWidgetControllerParams& InControllerParams);
	
public:
	UPROPERTY(BlueprintReadOnly, Category="UI")
	TObjectPtr<UAuraUserWidget> AuraHUD;
	
protected:

private:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UAuraUserWidget> HudClass;

	UPROPERTY(Transient)
	TObjectPtr<UHUDWidgetController> HUDWidgetController;

	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<UHUDWidgetController> HUDWidgetControllerClass;
};
