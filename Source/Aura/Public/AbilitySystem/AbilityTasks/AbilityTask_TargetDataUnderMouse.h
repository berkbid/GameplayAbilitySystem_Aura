// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_TargetDataUnderMouse.generated.h"

class UGameplayAbility;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataDelegate, const FGameplayAbilityTargetDataHandle&, Data);

/**
 * 
 */
UCLASS()
class AURA_API UAbilityTask_TargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
public:
	/** 
	 * Get target data under mouse.
	 *
	 * @param TaskInstanceName Set to override the name of this task, for later querying
	 */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (DisplayName="TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_TargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility, FName TaskInstanceName);

protected:
	virtual void Activate() override;
	
	virtual void OnDestroy(bool AbilityIsEnding) override;
	
	virtual bool ShouldReplicateDataToServer() const;
	
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataDelegate ValidMouseTargetData;

private:
	void SendTargetDataToServer();

	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);

	FDelegateHandle OnTargetDataReadDelegateHandle;
};
