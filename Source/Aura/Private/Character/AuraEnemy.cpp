// Copyright Berkeley Bidwell

#include "Character/AuraEnemy.h"
#include "Components/CapsuleComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	// Could use mesh if we want to for cursor over events
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	// Can we set this in constructor of aura ability system component?
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
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
	
/*
	// This seems to not even be necessary, ASC automatically inits this with correct owner/avatar on initializecomponent
	check(AbilitySystemComponent);
	// All clients and server are in here
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
*/
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
