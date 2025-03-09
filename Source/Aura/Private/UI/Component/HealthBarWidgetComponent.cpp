// Copyright Berkeley Bidwell

#include "UI/Component/HealthBarWidgetComponent.h"
#include "UI/WidgetController/EnemyWidgetController.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(HealthBarWidgetComponent)

UHealthBarWidgetComponent::UHealthBarWidgetComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetWidgetSpace(EWidgetSpace::Screen);
	SetDrawAtDesiredSize(true);
	
	SetGenerateOverlapEvents(false);
	SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SetCollisionResponseToAllChannels(ECR_Ignore);
}

void UHealthBarWidgetComponent::InitWidget()
{
	Super::InitWidget();

	// Could do this here, but doing on AuraEnemy BeginPlay() so we don't need to reference AuraEnemy from this class
	/*
	if (UAuraUserWidget* AuraWidgetObject = Cast<UAuraUserWidget>(GetUserWidgetObject()))
	{
		if (AAuraEnemy* EnemyOwner = Cast<AAuraEnemy>(GetOwner()))
		{
			// Set the widget controller for the widget
			AuraWidgetObject->SetWidgetController(EnemyOwner->GetEnemyWidgetController());
		}
	}
	*/
}
