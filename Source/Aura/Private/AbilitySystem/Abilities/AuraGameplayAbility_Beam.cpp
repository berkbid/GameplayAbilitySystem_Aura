// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraGameplayAbility_Beam.h"
#include "GameFramework/Character.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameplayAbility_Beam)

bool UAuraGameplayAbility_Beam::OnValidMouseData(const FHitResult& Hit)
{
	const bool bIsBlockingHit = Hit.bBlockingHit;
	if (bIsBlockingHit)
	{
		MouseHitLocation = Hit.Location;
		MouseHitActor = Hit.GetActor();
	}
	else
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
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
