// Copyright Berkeley Bidwell

#pragma once

#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "AsyncAction_CooldownChange.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

class UObject;
class UAbilitySystemComponent;
struct FFrame;

/**
 * 
 */
UCLASS(MinimalAPI)
class UAsyncAction_CooldownChange : public UCancellableAsyncAction
{
	GENERATED_BODY()
	
public:
	// Waits for the experience to be determined and loaded
	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true"))
	static AURA_API UAsyncAction_CooldownChange* WaitForCooldownChange(UAbilitySystemComponent* InASC, const FGameplayTag& InCooldownTag);

	//~UBlueprintAsyncActionBase interface
	AURA_API virtual void Activate() override;
	AURA_API virtual void SetReadyToDestroy() override;
	//~End of UBlueprintAsyncActionBase interface
	
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;
	
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

protected:
	AURA_API void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
	
protected:
	TWeakObjectPtr<UAbilitySystemComponent> ASC;

	FGameplayTag CooldownTag;
};
