// Copyright Berkeley Bidwell

#pragma once

#include "ModularAIController.h"
#include "AuraAIController.generated.h"

class UObject;
class UBehaviorTreeComponent;
class APawn;

/**
 * AAuraAIController
 */
UCLASS()
class AURA_API AAuraAIController : public AModularAIController
{
	GENERATED_BODY()
	
public:
	AAuraAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	
protected:
	//~ Begin AController Interface
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;
	//~ End AController Interface
protected:
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
};
