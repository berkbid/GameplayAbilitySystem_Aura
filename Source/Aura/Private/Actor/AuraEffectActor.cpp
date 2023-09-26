// Copyright Berkeley Bidwell

#include "Actor/AuraEffectActor.h"

#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());

	bReplicates = true;
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	// Only server should handle this, if client gets overlap but server doesn't then there is no way for server to correct this
	if (GetLocalRole() == ROLE_Authority)
	{
		Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::BeginOverlap);
		Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::EndOverlap);
	}
}

void AAuraEffectActor::BeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin Overlap: %s"), *GetName());
	// TODO: Change to apply a Gameplay Effect.
	if (const IAbilitySystemInterface* AbilitySystemInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		if (const UAbilitySystemComponent* AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent())
		{
			if (const UAuraAttributeSet* AuraAttributes = Cast<UAuraAttributeSet>(AbilitySystemComponent->GetAttributeSet(UAuraAttributeSet::StaticClass())))
			{
				// Casting away the const as a hack
				UE_LOG(LogTemp, Warning, TEXT("Setting new health: %s"), *GetName());
				const_cast<UAuraAttributeSet*>(AuraAttributes)->SetHealth(AuraAttributes->GetHealth() + 25.f);
				Destroy();
			}
		}
	}
}

void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}

