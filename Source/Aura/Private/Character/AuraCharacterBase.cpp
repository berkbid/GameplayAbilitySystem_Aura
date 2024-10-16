// Copyright Berkeley Bidwell

#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"

AAuraCharacterBase::AAuraCharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	if (WeaponTipSocketName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("%s missing WeaponTipSocketName property"), *GetName());
	}
	
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyGameplayEffectToSelf(DefaultPrimaryAttributesClass, 1.f);
	ApplyGameplayEffectToSelf(DefaultSecondaryAttributesClass, 1.f);
	// Set vital after secondary because we want to set health/mana equal to max health/mana
	ApplyGameplayEffectToSelf(DefaultVitalAttributesClass, 1.f);
}

void AAuraCharacterBase::ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& InGameplayEffectClass, float Level) const
{
	if (!HasAuthority())
	{
		return;
	}
	
	check(IsValid(GetAbilitySystemComponent()));
	check(InGameplayEffectClass);
	
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(InGameplayEffectClass, Level, ContextHandle);
	
	if (SpecHandle.Data.IsValid())
	{
		if (const FGameplayEffectSpec* EffectSpec = SpecHandle.Data.Get())
		{
			GetAbilitySystemComponent()->ApplyGameplayEffectSpecToSelf(*EffectSpec);
		}
	}
}

void AAuraCharacterBase::AddCharacterAbilities() const
{
	if (!HasAuthority())
	{
		return;
	}

	// Have ability system component add abilities
	UAuraAbilitySystemComponent* ASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
	ASC->AddCharacterAbilities(StartupAbilities);
	
}
