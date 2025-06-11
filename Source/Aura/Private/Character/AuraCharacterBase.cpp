// Copyright Berkeley Bidwell

#include "Character/AuraCharacterBase.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraCharacterBase)

AAuraCharacterBase::AAuraCharacterBase(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetNetCullDistanceSquared(900000000.0f);
	
	// Set capsule to ignore collision with camera channel
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	// Mesh handles overlaps, disable capsule overlap so there aren't double overlap events
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	
	// Set mesh to ignore collision with camera channel
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	// Set mesh to Overlap with projectiles
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile, ECR_Overlap);
	// Needs to generate overlap events for projectile to overlap
	GetMesh()->SetGenerateOverlapEvents(true);
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Weapon->SetupAttachment(GetMesh(), FName("WeaponHandSocket"));

	// Default name for weapon skeletal mesh tip socket
	WeaponTipSocketName = FName("TipSocket");
}

FVector AAuraCharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon))
	{
		check(IsValid(Weapon));
		UE_CLOG(WeaponTipSocketName.IsNone(), LogTemp, Error, TEXT("%s missing WeaponTipSocketName property"), *GetName());
		
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	
	return FVector();
}

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation() const
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die()
{
	// Called on server

	// TODO: Should we check if !bDead ?
	
	// Drop weapon
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));

	// Allow clients as well to handle death
	MulticastHandleDeath();
}

bool AAuraCharacterBase::IsDead_Implementation() const
{
	return !IsValid(this) || bDead;
}

AActor* AAuraCharacterBase::GetAvatar_Implementation()
{
	return this;
}

FTaggedMontage AAuraCharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) const
{
	for (const FTaggedMontage& TaggedMontage : AttackMontages)
	{
		if (TaggedMontage.MontageTag == MontageTag)
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	// Handle client and server actions for death
	
	// Set variable state for server and client instead of replicating it
	bDead = true;

	if (ensure(DeathSound))
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), GetActorRotation());
	}
	
	// Ragdoll weapon
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	// Ragdoll character mesh
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	// Disable collision on capsule
	// Having the capsule block worldstatic keeps the health bar component from falling downwards on client
	//GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	
	Dissolve();
}

void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
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

void AAuraCharacterBase::Dissolve()
{
	TArray<UMaterialInstanceDynamic*> DynamicMaterialInstances;
	
	if (IsValid(BodyDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(BodyDissolveMaterialInstance, this);
		
		GetMesh()->SetMaterial(0, DynamicMatInst);

		DynamicMaterialInstances.Add(DynamicMatInst);
	}

	if (IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		
		Weapon->SetMaterial(0, DynamicMatInst);
		
		DynamicMaterialInstances.Add(DynamicMatInst);
	}
	
	if (!DynamicMaterialInstances.IsEmpty())
	{
		StartDissolveTimeline(DynamicMaterialInstances);
	}
}
