// Copyright Berkeley Bidwell

#pragma once

#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "PassiveNiagaraComponent.generated.h"

class UObject;
class AActor;

/**
 * 
 */
UCLASS()
class UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
	
public:
	UPassiveNiagaraComponent();
	
protected:
	virtual void BeginPlay() override;
	
	void OnPassiveActivated(const FGameplayTag& AbilityTag, bool bActivate);
	
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
	
public:
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;
};
