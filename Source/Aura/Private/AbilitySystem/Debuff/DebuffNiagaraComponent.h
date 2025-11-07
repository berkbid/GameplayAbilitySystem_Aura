// Copyright Berkeley Bidwell

#pragma once

#include "NiagaraComponent.h"
#include "GameplayTagContainer.h"
#include "DebuffNiagaraComponent.generated.h"

class UObject;
class AActor;

UCLASS()
class UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	UDebuffNiagaraComponent();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Aura|Debuff")
	FGameplayTag DebuffTag;
	
protected:
	virtual void BeginPlay() override;
	
	void OnDebuffTagChanged(const FGameplayTag GameplayTag, int32 TagCount);
	
	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor);
};
