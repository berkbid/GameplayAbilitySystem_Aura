// Copyright Berkeley Bidwell

#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Interaction/CombatInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PassiveNiagaraComponent)

UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bAutoActivate = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ICombatInterface* CombatInterface = CastChecked<ICombatInterface>(GetOwner());
	
	// Get the ability system component from the owner
	if (UAuraAbilitySystemComponent* ASC = Cast<UAuraAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner())))
	{
		if (!ASC->ActivatePassiveEffect.IsBoundToObject(this))
		{
			ASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivated);
		}
	}
	else
	{
		// Bind to the OnASCRegistered delegate
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* ASC)
		{
			if (UAuraAbilitySystemComponent* AuraASC = Cast<UAuraAbilitySystemComponent>(ASC))
			{
				if (!AuraASC->ActivatePassiveEffect.IsBoundToObject(this))
				{
					AuraASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivated);
				}
			}
		});
	}
	
	// Bind to owner death to deactivate
	if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UPassiveNiagaraComponent::OnOwnerDeath))
	{
		CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UPassiveNiagaraComponent::OnOwnerDeath);
	}
}

void UPassiveNiagaraComponent::OnPassiveActivated(const FGameplayTag& AbilityTag, bool bActivate)
{
	if (AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		if (bActivate && !IsActive())
		{
			Activate();
		}
		else if (!bActivate && IsActive())
		{
			Deactivate();
		}
	}
}

void UPassiveNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
