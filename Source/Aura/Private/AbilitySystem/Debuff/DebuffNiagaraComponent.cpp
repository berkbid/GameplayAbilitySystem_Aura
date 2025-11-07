// Copyright Berkeley Bidwell

#include "DebuffNiagaraComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Interaction/CombatInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DebuffNiagaraComponent)

UDebuffNiagaraComponent::UDebuffNiagaraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	bAutoActivate = false;
}

void UDebuffNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();

	ICombatInterface* CombatInterface = CastChecked<ICombatInterface>(GetOwner());
	
	// Get the ability system component from the owner
	if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner()))
	{
		FOnGameplayEffectTagCountChanged& TagDelegate = ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved);
		TagDelegate.AddUObject(this, &UDebuffNiagaraComponent::OnDebuffTagChanged);
	}
	else
	{
		// Bind to the OnASCRegistered delegate
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this, [this](UAbilitySystemComponent* ASC)
		{
			FOnGameplayEffectTagCountChanged& TagDelegate = ASC->RegisterGameplayTagEvent(DebuffTag, EGameplayTagEventType::NewOrRemoved);
			TagDelegate.AddUObject(this, &UDebuffNiagaraComponent::OnDebuffTagChanged);
		});
	}
	
	// Bind to owner death to deactivate
	CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
	CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UDebuffNiagaraComponent::OnOwnerDeath);
}

void UDebuffNiagaraComponent::OnDebuffTagChanged(const FGameplayTag GameplayTag, int32 TagCount)
{
	if (TagCount > 0)
	{
		if (AActor* Owner = GetOwner(); IsValid(Owner) && Owner->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsDead(Owner))
		{
			// Activate the Niagara effect when debuff is applied and owner is not dead
			Activate();
		}
	}
	else
	{
		// Deactivate when debuff is removed
		Deactivate();
	}
}

void UDebuffNiagaraComponent::OnOwnerDeath(AActor* DeadActor)
{
	Deactivate();
}
