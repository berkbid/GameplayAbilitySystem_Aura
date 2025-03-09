// Copyright Berkeley Bidwell

#include "Character/AuraEnemy.h"
#include "Components/CapsuleComponent.h"
#include "UI/Component/HealthBarWidgetComponent.h"
#include "UI/WidgetController/EnemyWidgetController.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraEnemy)

AAuraEnemy::AAuraEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create widget component for health bar and attach to capsule
	HealthBar = CreateDefaultSubobject<UHealthBarWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetCapsuleComponent());
	
	// Could use mesh if we want to for cursor over events
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	// Can we set this in constructor of aura ability system component?
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Apparently create default subobject can be used for regular objects that aren't components
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// Set stencil values for highlight post process
	GetMesh()->CustomDepthStencilValue = CUSTOM_DEPTH_RED;
	if (Weapon)
	{
		Weapon->CustomDepthStencilValue = CUSTOM_DEPTH_RED;
	}
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();

	// Set the widget controller on the health bar component's widget
	if (UAuraUserWidget* HealthBarAuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		HealthBarAuraUserWidget->SetWidgetController(GetEnemyWidgetController());
	}
}

void AAuraEnemy::InitAbilityActorInfo()
{
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	// Only doing this as server (values get replicated to clients), course does not do this check
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}

UEnemyWidgetController* AAuraEnemy::GetEnemyWidgetController()
{
	if (!EnemyWidgetController)
	{
		checkf(EnemyWidgetControllerClass, TEXT("Enemy missing enemy widget controller class reference! Please fill out B_EnemyBase"));
		
		EnemyWidgetController = NewObject<UEnemyWidgetController>(this, EnemyWidgetControllerClass);

		FWidgetControllerParams WidgetParams;
		WidgetParams.AbilitySystemComponent = AbilitySystemComponent;
		WidgetParams.AttributeSet = AttributeSet;
		WidgetParams.PlayerState = GetPlayerState();
		
		EnemyWidgetController->SetWidgetControllerParams(WidgetParams);
	}
	
	return EnemyWidgetController;
}

void AAuraEnemy::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	// Highlights enemy with post process material in post process volume using stencil index 250
	GetMesh()->SetRenderCustomDepth(true);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
	}
}

void AAuraEnemy::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	
	// UnHighlights enemy with post process material in post process volume
	GetMesh()->SetRenderCustomDepth(false);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}
