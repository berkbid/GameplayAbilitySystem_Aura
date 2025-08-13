// Copyright Berkeley Bidwell

#pragma once

#include "UI/Widget/AuraUserWidget.h"
#include "AuraSpellTree.generated.h"

class UObject;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSpellClicked, UObject*, SpellButton);

/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraSpellTree : public UAuraUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnSpellClicked OnSpellClicked;
	
};
