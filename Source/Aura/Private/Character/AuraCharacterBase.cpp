// Copyright Berkeley Bidwell

#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyGameplayEffectToSelf(DefaultPrimaryAttributesClass, 1.f);
	ApplyGameplayEffectToSelf(DefaultSecondaryAttributesClass, 1.f);
}

void AAuraCharacterBase::ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& InGameplayEffectClass, float Level) const
{
	if (!HasAuthority())
	{
		return;
	}
	
	check(IsValid(GetAbilitySystemComponent()));
	check(InGameplayEffectClass);
	
	const FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InGameplayEffectClass, Level, ContextHandle);
	
	if (SpecHandle.Data.IsValid())
	{
		if (const FGameplayEffectSpec* EffectSpec = SpecHandle.Data.Get())
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectSpec);
		}
	}
}
