// Copyright Berkeley Bidwell

#include "AI/AuraAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAIController)

AAuraAIController::AAuraAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>("Blackboard");
	check(Blackboard);
	
	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComponent");
	check(BehaviorTreeComponent);
}

void AAuraAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void AAuraAIController::OnUnPossess()
{
	Super::OnUnPossess();
}
