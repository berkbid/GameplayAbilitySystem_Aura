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

void UAuraGA_CastProjectile::CastProjectile()
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
	
	FTransform SpawnTransform;
	// Spawn at socket location of weapon of who is calling this
	SpawnTransform.SetLocation(CombatInterfaceAvatar->GetCombatSocketLocation());
	// TODO: Set projectile rotation

	
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
