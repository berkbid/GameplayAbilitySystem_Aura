// Copyright Berkeley Bidwell

#include "UI/Subsystem/AuraUIManagerSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraUIManagerSubsystem)

UAuraUIManagerSubsystem::UAuraUIManagerSubsystem()
{
}

void UAuraUIManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// TODO: Lyra does stuff here to add tick and sync hud visibility to root layout
}

void UAuraUIManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();
}
