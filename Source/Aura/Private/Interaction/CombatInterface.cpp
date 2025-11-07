// Copyright Berkeley Bidwell

#include "Interaction/CombatInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CombatInterface)

int32 ICombatInterface::GetPlayerLevel_Implementation() const
{
	return 1;
}

FVector ICombatInterface::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	return FVector::ZeroVector;
}

void ICombatInterface::Die(const FVector& DeathImpulse)
{
}
