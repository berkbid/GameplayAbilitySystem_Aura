// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraGameplayAbility_FireBolt.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Actor/AuraProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interaction/CombatInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameplayAbility_FireBolt)

void UAuraGameplayAbility_FireBolt::SpawnProjectiles(const FVector& ProjectileLocation, const FGameplayTag& SocketTag, bool bOverridePitch, float PitchOverride, AActor* HomingTarget)
{
	const bool bIsServer = HasAuthority(&CurrentActivationInfo);
	if (!bIsServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast projectile: Not server, returning"));
		return;
	}

	checkf(AuraProjectileClass, TEXT("Missing projectile class for: %s"), *GetName());
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	
	if (!AvatarActor || !AvatarActor->Implements<UCombatInterface>())
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to cast projectile with avatar: %s that does not implement combat interface!"), AvatarActor ? *AvatarActor->GetName() : TEXT("NONE"));
		return;
	}

	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(AvatarActor, SocketTag);

	FRotator ForwardRotation = (ProjectileLocation - SocketLocation).Rotation();
	//ForwardRotation.Pitch = bOverridePitch ? PitchOverride : ForwardRotation.Pitch;
	ForwardRotation.Pitch = bOverridePitch ? PitchOverride : 0.f;

	const int32 EffectiveMaxNumProjectiles = FMath::Min(MaxNumProjectiles, GetAbilityLevel());
	const FVector Forward = ForwardRotation.Vector();
	const TArray<FRotator> Rotations = UAuraAbilitySystemLibrary::EvenlySpacedRotators(Forward, FVector::UpVector, ProjectileSpread, EffectiveMaxNumProjectiles);

	for (const FRotator& Rotation : Rotations)
	{
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(SocketLocation);
		SpawnTransform.SetRotation(Rotation.Quaternion());

		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
		AuraProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(AvatarActor),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		if (IsValid(Projectile))
		{
			if (!ensure(DamageEffectClass))
			{
				UE_LOG(LogTemp, Warning, TEXT("Gameplay Ability for projectile missing damage effect class for damage gameplay effect"));
			}

			// Set the damage effect params that projectile can use for damage (no target actor set)
			Projectile->DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();

			if (bLaunchHomingProjectiles)
			{
				if (HomingTarget && HomingTarget->Implements<UCombatInterface>())
				{
					Projectile->GetProjectileMovement()->HomingTargetComponent = HomingTarget->GetRootComponent();
				}
				else
				{
					Projectile->HomingTargetSceneComponent = NewObject<USceneComponent>(USceneComponent::StaticClass());
					Projectile->HomingTargetSceneComponent->SetWorldLocation(ProjectileLocation);
					Projectile->GetProjectileMovement()->HomingTargetComponent = Projectile->HomingTargetSceneComponent;
				}
			
				Projectile->GetProjectileMovement()->HomingAccelerationMagnitude = FMath::FRandRange(HomingAccelerationMin, HomingAccelerationMax);
				Projectile->GetProjectileMovement()->bIsHomingProjectile = true;
			}

			
			// Finish spawning projectile from deferred spawn
			Projectile->FinishSpawning(SpawnTransform);	
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to cast projectile"));
		}
	}
}

FString UAuraGameplayAbility_FireBolt::GetDescription(int32 Level) const
{
	return GetDescriptionInternal(FString("FIRE BOLT:"), Level);
}

FString UAuraGameplayAbility_FireBolt::GetNextLevelDescription(int32 Level) const 
{
	return GetDescriptionInternal(FString("NEXT LEVEL:"), Level);
}

FString UAuraGameplayAbility_FireBolt::GetDescriptionInternal(const FString& InTitle, int32 Level) const
{
	float FireDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const int32 NumFireBolts = FMath::Min(Level, MaxNumProjectiles);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// Title and level
		"<Title>%s %d</>\n\n"
		// Level
		"<Small>Level: </><Level>%d</>\n"
		// Mana cost
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// Num projectiles
		"Launches %d bolt%s of fire, "
		// Damage
		"exploding on impact an dealing: <Damage>%.1f</>"
		" fire damage with a chance to burn\n\n"),
		*InTitle, Level, Level, ManaCost, Cooldown, NumFireBolts, NumFireBolts > 1 ? TEXT("s") : TEXT(""), FireDamage);
}
