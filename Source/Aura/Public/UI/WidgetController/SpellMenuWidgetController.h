// Copyright Berkeley Bidwell

#pragma once

#include "UI/WidgetController/AuraWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

class UObject;
struct FGameplayTag;

/**
 * 
 */
UCLASS(MinimalAPI)
class USpellMenuWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAuraWidgetController interface
	AURA_API virtual void BroadcastInitialValues() override;
	AURA_API virtual void BindCallBacksToDependencies() override;
	// ~ End UAuraWidgetController interface

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	AURA_API void SpendOrRefundSpellPoint(const FGameplayTag& AbilityTag, int32 SpellPoints) const;
	
	UFUNCTION(BlueprintCallable)
	AURA_API bool GetDescriptionsByAbilityTag(const FGameplayTag& AbilityTag, const int32 Level, FString& OutDescription, FString& OutNextLevelDescription);

	UFUNCTION(BlueprintCallable)
	AURA_API void EquipAbility(const FGameplayTag& AbilityTag, const FGameplayTag& InputTag);

protected:

protected:
	UPROPERTY(BlueprintAssignable, Category="GAS|SpellPoints")
	FOnPlayerStatChangedSignature OnSpellPointsChanged;
};
