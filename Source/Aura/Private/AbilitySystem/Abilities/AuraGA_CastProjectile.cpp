// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraGA_CastProjectile.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"

void UAuraGA_CastProjectile::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                             const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                             const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// Both server and client are in here
	//const bool bIsServer = HasAuthority(&ActivationInfo);
	//const bool bIsServer = HasAuthority(&CurrentActivationInfo);
	
}

bool UAuraGA_CastProjectile::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UAuraGA_CastProjectile::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        bool bReplicateEndAbility, bool bWasCancelled)
{
	// Referenced from LyraGameplayAbility_RangedWeapon
	if (IsEndAbilityValid(Handle, ActorInfo))
	{
		if (ScopeLockCount > 0)
		{
			WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &ThisClass::EndAbility, Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled));
			return;
		}
		
		// Custom code here

		
		Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UAuraGA_CastProjectile::CastProjectile(const FVector& ProjectileTargetLocation)
{
	// Spawn projectile on server
	const bool bIsServer = HasAuthority(&CurrentActivationInfo);
	//const bool bIsServer = GetCurrentActorInfo()->IsNetAuthority();
	//const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority();
	
	if (!bIsServer)
	{
		UE_LOG(LogTemp, Display, TEXT("Cast projectile: Not server, returning"));
		return;
	}
	UE_LOG(LogTemp, Display, TEXT("Cast projectile: Server"));
	

	checkf(AuraProjectileClass, TEXT("Missing projectile class for: %s"), *GetName());
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	
	ICombatInterface* CombatInterfaceAvatar = Cast<ICombatInterface>(AvatarActor);
	if (!CombatInterfaceAvatar)
	{
		UE_LOG(LogTemp, Error, TEXT("Attempting to cast projectile with avatar: %s that does not implement combat interface!"), AvatarActor ? *AvatarActor->GetName() : TEXT("NONE"));
		return;
	}
	
	// See AbilityTask_SpawnActor for interesting code
	const FVector SocketLocation = CombatInterfaceAvatar->GetCombatSocketLocation();
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation();
	// Parallel to ground
	Rotation.Pitch = 0.f;
	
	FTransform SpawnTransform;
	// Spawn at socket location of weapon of who is calling this
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
		// TODO: Give projectile gameplay effect spec for causing damage
		
		
		// Finish spawning projectile from deferred spawn
		Projectile->FinishSpawning(SpawnTransform);	
	}
}

void UAuraGA_CastProjectile::UpdateTargetLocation(const FVector& InTargetLocation)
{
	TargetLocation = InTargetLocation;

	// Update the avatar actor's motion warping component's facing target for anim montage to utilize when motion warping
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	
	if (AvatarActor && AvatarActor->Implements<UCombatInterface>())
	{
		ICombatInterface::Execute_SetFacingTarget(AvatarActor, TargetLocation);
	}
}
