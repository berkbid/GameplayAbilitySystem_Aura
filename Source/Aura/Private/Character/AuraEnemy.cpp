// Copyright Berkeley Bidwell

#include "Character/AuraEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	// Could use mesh if we want to for cursor over events
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	//GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	
	GetMesh()->CustomDepthStencilValue = CUSTOM_DEPTH_RED;
	if (Weapon)
	{
		Weapon->CustomDepthStencilValue = CUSTOM_DEPTH_RED;
	}
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
