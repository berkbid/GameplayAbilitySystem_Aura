// Copyright Berkeley Bidwell

#include "UI/Common/AuraListView.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraListView)

UAuraListView::UAuraListView(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	
}

#if WITH_EDITOR

void UAuraListView::ValidateCompiledDefaults(IWidgetCompilerLog& InCompileLog) const
{
	Super::ValidateCompiledDefaults(InCompileLog);
	
}

#endif

UUserWidget& UAuraListView::OnGenerateEntryWidgetInternal(UObject* Item, TSubclassOf<UUserWidget> DesiredEntryClass,
	const TSharedRef<STableViewBase>& OwnerTable)
{
	return Super::OnGenerateEntryWidgetInternal(Item, DesiredEntryClass, OwnerTable);
}
