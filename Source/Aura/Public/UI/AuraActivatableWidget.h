// Copyright Berkeley Bidwell

#pragma once

#include "CommonActivatableWidget.h"
#include "AuraActivatableWidget.generated.h"

struct FUIInputConfig;
class IWidgetCompilerLog;
class UWidgetTree;

UENUM(BlueprintType)
enum class EAuraWidgetInputMode : uint8
{
	Default,
	GameAndMenu,
	Game,
	Menu
};

/**
 * An activatable widget that automatically drives the desired input config when activated
 */
UCLASS(Abstract, Blueprintable)
class AURA_API UAuraActivatableWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
public:
	UAuraActivatableWidget(const FObjectInitializer& ObjectInitializer);
	
public:
	
	//~UCommonActivatableWidget interface
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;
	//~End of UCommonActivatableWidget interface

#if WITH_EDITOR
	virtual void ValidateCompiledWidgetTree(const UWidgetTree& BlueprintWidgetTree, IWidgetCompilerLog& CompileLog) const override;
#endif
	
protected:
	/** The desired input mode to use while this UI is activated, for example do you want key presses to still reach the game/player controller? */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EAuraWidgetInputMode InputConfig = EAuraWidgetInputMode::Default;

	/** The desired mouse behavior when the game gets input. */
	UPROPERTY(EditDefaultsOnly, Category = Input)
	EMouseCaptureMode GameMouseCaptureMode = EMouseCaptureMode::CapturePermanently;

	
};
