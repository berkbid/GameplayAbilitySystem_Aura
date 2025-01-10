// Copyright Berkeley Bidwell

#pragma once

#include "Components/ListView.h"
#include "AuraListView.generated.h"

class UUserWidget;

/**
 * 
 */
UCLASS(meta = (DisableNativeTick))
class AURA_API UAuraListView : public UListView
{
	GENERATED_BODY()
	
public:
	UAuraListView(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const override;
#endif

protected:
	virtual UUserWidget& OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass, const TSharedRef<STableViewBase>& OwnerTable) override;
	//virtual bool OnIsSelectableOrNavigableInternal(UObject* SelectedItem) override;

	
};
