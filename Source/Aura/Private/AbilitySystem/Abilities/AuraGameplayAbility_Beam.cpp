// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraGameplayAbility_Beam.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Aura/Aura.h"
#include "GameFramework/Character.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameplayAbility_Beam)

bool UAuraGameplayAbility_Beam::OnValidMouseData(const FHitResult& Hit)
{
	const bool bIsBlockingHit = Hit.bBlockingHit;
	if (bIsBlockingHit)
	{
		MouseHitLocation = Hit.Location;
		
		// I don't think we want to set this here, it is really set on anim montage event received
		MouseHitActor = Hit.GetActor();
	}
	else
	{
		// This will mimic endabilitylocally 
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, false, false);
		//EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
	return bIsBlockingHit;
}

void UAuraGameplayAbility_Beam::StoreOwnerVariables()
{
	if (CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor.Get());
	}
}

void UAuraGameplayAbility_Beam::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	check(OwnerCharacter && OwnerCharacter->Implements<UCombatInterface>());
	
	if (USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
	{
		TArray<AActor*> IgnoreActors;
		IgnoreActors.Add(OwnerCharacter);
		
		FHitResult HitResult;
		const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket"));
		
		UKismetSystemLibrary::SphereTraceSingle(OwnerCharacter, SocketLocation, BeamTargetLocation, 10.f, 
			TraceTypeQuery1, false, IgnoreActors, EDrawDebugTrace::None, HitResult, true);
		
		if (HitResult.bBlockingHit)
		{
			MouseHitLocation = HitResult.ImpactPoint;
			MouseHitActor = HitResult.GetActor();
			
			UE_LOG(LogTemp, Error, TEXT("Trace hit actor: %s"), *GetNameSafe(MouseHitActor));
			
			if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
			{
				if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraGameplayAbility_Beam::PrimaryTargetDied))
				{
					CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraGameplayAbility_Beam::PrimaryTargetDied);
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Already bound to first target ondeath: %s"), *GetNameSafe(MouseHitActor));
				}
			}
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Trace did not hit anything"));
		}
	}
}

void UAuraGameplayAbility_Beam::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	// Only propagate beam if the mouse hit actor is a valid enemy that is not dead
	if (!MouseHitActor || !MouseHitActor->ActorHasTag(ACTOR_TAG_ENEMY) || !MouseHitActor->Implements<UCombatInterface>() || ICombatInterface::Execute_IsDead(MouseHitActor))
	{
		return;
	}

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());
	ActorsToIgnore.Add(MouseHitActor);
	
	const FVector Origin = MouseHitActor->GetActorLocation();
	
	TArray<AActor*> OverlappingActors;
	UAuraAbilitySystemLibrary::GetLivePlayersWithinRadius(GetAvatarActorFromActorInfo(), OverlappingActors, ActorsToIgnore, 850.f, Origin);
	
	// Ignore players
	for (int32 ActorIdx = OverlappingActors.Num() - 1; ActorIdx >= 0; --ActorIdx)
	{
		if (OverlappingActors[ActorIdx]->ActorHasTag(ACTOR_TAG_PLAYER))
		{
			OverlappingActors.RemoveAt(ActorIdx);
		}
	}
	
	const int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	//const int32 NumAdditionalTargets = 5;
	
	UAuraAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, Origin);
	
	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &UAuraGameplayAbility_Beam::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &UAuraGameplayAbility_Beam::AdditionalTargetDied);
			}
		}
	}
}

void UAuraGameplayAbility_Beam::ClearAllTargets()
{
	if (MouseHitActor)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(MouseHitActor))
		{
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraGameplayAbility_Beam::PrimaryTargetDied);
		}
		MouseHitActor = nullptr;
	}
	
	for (const TPair<AActor*, FGameplayCueParameters>& Pair : AdditionalTargetsMap)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Pair.Key))
		{
			CombatInterface->GetOnDeathDelegate().RemoveDynamic(this, &UAuraGameplayAbility_Beam::AdditionalTargetDied);
		}
	}
	AdditionalTargetsMap.Empty();
	
	OwnerPlayerController = nullptr;
	OwnerCharacter = nullptr;
}

FString UAuraGameplayAbility_Beam::GetDescription(int32 Level) const
{
	return GetDescriptionInternal(FString("CHAIN LIGHTNING:"), Level);
}

FString UAuraGameplayAbility_Beam::GetNextLevelDescription(int32 Level) const
{
	return GetDescriptionInternal(FString("NEXT LEVEL:"), Level);
}

FString UAuraGameplayAbility_Beam::GetDescriptionInternal(const FString& InTitle, int32 Level) const
{
	float LightningDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const int32 NumTargets = FMath::Min(Level - 1, MaxNumShockTargets) + 1;
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// Title and level
		"<Title>%s %d</>\n\n"
		// Level
		"<Small>Level: </><Level>%d</>\n"
		// Mana cost
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n"
		// Number targets
		"<Small>Targets: </><Cooldown>%d</>\n"
		// Damage
		"<Small>Damage: </><Damage>%.1f</>\n"
		// Chance to stun
		"<Small>Chance to stun: </>%.1f%%"),
		*InTitle, Level, Level, ManaCost, Cooldown, NumTargets, LightningDamage, DebuffChance);
}
