// Copyright Berkeley Bidwell

#include "Actor/AuraProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"

AAuraProjectile::AAuraProjectile()
{
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
	
	PrimaryActorTick.bCanEverTick = false;

	// Want the projectiles to replicate
	bReplicates = true;
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	
	SetLifeSpan(LifeSpan);
	
	UE_LOG(LogTemp, Warning, TEXT("AuraProjectile::BeginPlay - %s"), HasAuthority() ? TEXT("Authority") : TEXT("Not Authority"));
	
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
	// If client did not play destroyed effects because server destroyed projectile before client received overlap event
	//if (!HasAuthority() && !bPlayedDestroyedEffects)
	if (!bPlayedDestroyedEffects)
	{
		PlayDestroyEffects();
	}
	
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("AuraProjectile::OnSphereOverlap - %s, %s overlap with %s"), HasAuthority() ? TEXT("Authority") : TEXT("Not Authority"),
																OverlappedComponent ? *OverlappedComponent->GetName() : TEXT("NONE"),
																OtherComp ? *OtherComp->GetName() : TEXT("NONE"));
	if (!bPlayedDestroyedEffects)
	{
		PlayDestroyEffects();
	}
	
	// Only server should destroy
	if (HasAuthority())
	{
		Destroy();
	}
	// else
	// {
	// 	bPlayedDestroyedEffects = true;
	// }
}

void AAuraProjectile::PlayDestroyEffects()
{
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
