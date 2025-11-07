// Copyright Berkeley Bidwell

#pragma once

#include "UI/WidgetController/AuraWidgetController.h"
#include "GameplayTagContainer.h"
#include "HUDWidgetController.generated.h"

class UAuraUserWidget;
class UDataTable;
class UTexture2D;
class UObject;
class UGameplayAbility;
class UAuraAbilitySystemComponent;
struct FOnAttributeChangeData;
struct FGameplayTag;

USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAuraUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UTexture2D> Image = nullptr;
	
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature, const FUIWidgetRow&, WidgetRow);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAbilityCommitted, UGameplayAbility*, Ability);
/**
 * 
 */
UCLASS(MinimalAPI)
class UHUDWidgetController : public UAuraWidgetController
{
	GENERATED_BODY()
	
public:
	// ~ Begin UAuraWidgetController interface
	AURA_API virtual void BindCallBacksToDependencies() override;
	AURA_API virtual void BroadcastInitialValues() override;
	// ~ End UAuraWidgetController interface
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnManaChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxManaChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Messages")
	FMessageWidgetRowSignature MessageWidgetRow;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Abilities")
	FOnAbilityCommitted OnAbilityCommitted;

	UPROPERTY(BlueprintAssignable, Category="GAS|Xp")
	FOnAttributeChangedSignature OnXpPercentChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnPlayerStatChangedSignature OnPlayerLevelChangedDelegate;
	
protected:
	AURA_API void OnXpChanged(int32 NewXp) const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Widget Data", meta=(RequiredAssetDataTags="RowStructure=/Script/Aura.UIWidgetRow"))
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	
	template<typename T>
	static AURA_API T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
};

template <typename T>
T* UHUDWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable ? DataTable->FindRow<T>(Tag.GetTagName(), TEXT("")) : nullptr;
}
