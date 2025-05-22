// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraGA_CastProjectile.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AuraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGA_CastProjectile)

class APawn;

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
	//const bool bIsServerNew = GetCurrentActorInfo()->IsNetAuthority();
	//const bool bIsServerNewNew = GetAvatarActorFromActorInfo()->HasAuthority();
	
	if (!bIsServer)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cast projectile: Not server, returning"));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Cast projectile: Server"));

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
	
	//DrawDebugSphere(GetWorld(), SocketLocation, 20, 30, FColor::Yellow, false, 5.f);

	// TODO: If the projectiletargetlocation is further than the socket location, use socket location, else use avatar actor location
	//FRotator Rotation = (ProjectileTargetLocation - AvatarActor->GetActorLocation()).Rotation();
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
		if (!ensure(DamageEffectClass))
		{
			UE_LOG(LogTemp, Warning, TEXT("Gameplay Ability for projectile missing damage effect class for damage gameplay effect"));
		}
		
		
		if (const UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo())
		//if (const UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo()))
		{
			// Set some extra fields in the effect context handle
			FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();
			EffectContextHandle.SetAbility(this);
			EffectContextHandle.AddSourceObject(Projectile);
			TArray<TWeakObjectPtr<AActor>> Actors;
			Actors.Add(Projectile);
			EffectContextHandle.AddActors(Actors);
			FHitResult HitResult;
			HitResult.Location = ProjectileTargetLocation;
			EffectContextHandle.AddHitResult(HitResult);
			
			// Create spec handle using the GE_Damage
			const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

			// Set the damage gameplay tag and damage magnitude KV pair on the spec handle since the GE damage uses "Set by caller" magnitude calculation type
			//const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
			//UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, 50.f);

			// Allows for multiple damage types
			for (const TPair<FGameplayTag, FScalableFloat>& Pair : DamageTypes)
			{
				// Get the damage multiplier using the Damage scalable float curve
				const float ScaledDamage = Pair.Value.GetValueAtLevel(GetAbilityLevel());
				
				// Set the damage magnitude on the spec handle since the GE uses set by caller for magnitude calculation type
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(Pair.Key, ScaledDamage);
			}
			
			// Give projectile gameplay effect spec for causing damage
			Projectile->DamageEffectSpecHandle = SpecHandle;
		}
		
		
		// Finish spawning projectile from deferred spawn
		Projectile->FinishSpawning(SpawnTransform);	
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to cast projectile"));
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
