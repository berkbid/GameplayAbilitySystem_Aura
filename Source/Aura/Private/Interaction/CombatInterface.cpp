// Copyright Berkeley Bidwell

#include "Interaction/CombatInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CombatInterface)

int32 ICombatInterface::GetPlayerLevel() const
{
	return 0;
}

FVector ICombatInterface::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	return FVector::ZeroVector;
}

void ICombatInterface::Die()
{
}
