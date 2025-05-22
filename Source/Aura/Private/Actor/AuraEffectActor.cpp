// Copyright Berkeley Bidwell

#include "Actor/AuraEffectActor.h"
#include "Components/SceneComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraEffectActor)

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>("SceneRoot");
	SetRootComponent(SceneRoot);

	bReplicates = true;
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	// Only execute on server ?
	if (!HasAuthority())
	{
		return;
	}
	
	// Only apply effect if supposed to
	if (TargetActor && TargetActor->ActorHasTag(ACTOR_TAG_ENEMY) && !bApplyEffectsToEnemies)
	{
		return;
	}
	
	if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		// Actor level specifies the curve level value for the gameplay effect if using curve table on a scalar modifier
		const FGameplayEffectSpecHandle EffectSpec = AbilitySystemComponent->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContext);

		//  Do we need to check if EffectSpec.Data.IsValid() before calling Get() ?
		if (const FGameplayEffectSpec* Spec = EffectSpec.Data.Get())
		{
			// Get gameplay effect to see what duration policy is
			if (const UGameplayEffect* GE = Spec->Def.Get())
			{
				const FActiveGameplayEffectHandle ActiveEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec);
				
				if (GE->DurationPolicy == EGameplayEffectDurationType::Infinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemovalOnEndOverlap)
				{
					// If ASC already has active handle, remove it first
					if (const FActiveGameplayEffectHandle* EffectHandle = ActiveEffectHandles.Find(AbilitySystemComponent))
					{
						// Remove a single stack
						AbilitySystemComponent->RemoveActiveGameplayEffect(*EffectHandle, 1);
					}
					
					ActiveEffectHandles.Add(AbilitySystemComponent, ActiveEffectHandle);
				}
				// Non-infinite GE duration policies can be destroyed only
				else if (bDestroyOnEffectApplication)
				{
					Destroy();
				}
			}
		}
	}
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	if (TargetActor && TargetActor->ActorHasTag(ACTOR_TAG_ENEMY) && !bApplyEffectsToEnemies)
	{
		return;
	}
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor && TargetActor->ActorHasTag(ACTOR_TAG_ENEMY) && !bApplyEffectsToEnemies)
	{
		return;
	}
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemovalOnEndOverlap)
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor))
		{
			if (const FActiveGameplayEffectHandle* EffectHandle = ActiveEffectHandles.Find(AbilitySystemComponent))
			{
				// Remove a single stack
				AbilitySystemComponent->RemoveActiveGameplayEffect(*EffectHandle, 1);
			}
			ActiveEffectHandles.Remove(AbilitySystemComponent);
		}
	}
}
