// Copyright Berkeley Bidwell

#include "Actor/AuraProjectile.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AuraLogChannels.h"
#include "Components/SphereComponent.h"
#include "Components/AudioComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraProjectile)

struct FHitResult;

AAuraProjectile::AAuraProjectile(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionObjectType(ECC_Projectile);
	Sphere->SetCollisionResponseToChannels(ECR_Ignore);
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Overlap);
	Sphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale = 0.f;
	// Automatically set to root component
	//ProjectileMovement->UpdatedComponent = Sphere;
	
	// Want the projectiles to replicate
	bReplicates = true;
	SetReplicatingMovement(true);
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeSpan);
	
	
	//UE_LOG(LogTemp, Warning, TEXT("AuraProjectile::BeginPlay(%s) - Transform: %s"), *GetClientServerContextString(this), *GetActorTransform().ToString());
	
	// Letting client bind to on overlap as well for impact sound and effect
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap);

	if (LoopingSound)
	{
		LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound, GetRootComponent());
	}
	
}

void AAuraProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	
	Sphere->OnComponentBeginOverlap.RemoveDynamic(this, &AAuraProjectile::OnSphereOverlap);
}

void AAuraProjectile::Destroyed()
{
	// If client did not play destroyed effects because server destroyed projectile before the client received overlap event
	//if (!HasAuthority() && !bPlayedDestroyedEffects)

	PlayDestroyEffects();
	
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// TODO: Why does client get on sphere overlap multiple times and server only once?
	// UE_LOG(LogTemp, Warning, TEXT("AuraProjectile::OnSphereOverlap - %s, %s overlap with component %s, and actor: %s"), HasAuthority() ? TEXT("Authority") : TEXT("Not Authority"),
	// 															OverlappedComponent ? *OverlappedComponent->GetName() : TEXT("NONE"),
	// 															OtherComp ? *OtherComp->GetName() : TEXT("NONE"),
	// 															OtherActor ? *OtherActor->GetName() : TEXT("NONE"));
	
	if (OtherActor == GetInstigator())
	{
		//UE_LOG(LogTemp, Warning, TEXT("(%s): Overlapped with instigator actor: %s, doing nothing."), *GetClientServerContextString(this), *GetNameSafe(GetInstigator()));
		return;
	}
	
	if (UAuraAbilitySystemLibrary::AreFriends(GetInstigator(), OtherActor))
	{
		//UE_LOG(LogTemp, Warning, TEXT("(%s): Overlapped with friend actor: %s, doing nothing."), *GetClientServerContextString(this), *GetNameSafe(OtherActor));
		return;
	}
	
	PlayDestroyEffects();
	
	// Only server should destroy
	if (HasAuthority())
	{
		// Pass gameplay effect to the other actor
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			// Set target ASC
			DamageEffectParams.TargetASC = TargetASC;

			// Set the death impulse and knockback force
			DamageEffectParams.UpdateImpulsesWithRotation(GetActorRotation());
			
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
		
		Destroy();
	}
}

void AAuraProjectile::PlayDestroyEffects()
{
	if (bPlayedDestroyedEffects)
	{
		return;
	}
	
	// Stop looping sound
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
		LoopingSoundComponent = nullptr;
	}
	
	// Play impact sound
	if (ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
	}
	
	// Play impact particle effect
	if (ImpactEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ImpactEffect, GetActorLocation());
	}
	
	bPlayedDestroyedEffects = true;
}
