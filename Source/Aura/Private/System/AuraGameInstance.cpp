// Copyright Berkeley Bidwell

#include "System/AuraGameInstance.h"
#include "CommonSessionSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameInstance)

UAuraGameInstance::UAuraGameInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UAuraGameInstance::Init()
{
	Super::Init();

	if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
	{
		SessionSubsystem->OnPreClientTravelEvent.AddUObject(this, &UAuraGameInstance::OnPreClientTravelToSession);
	}
}

void UAuraGameInstance::Shutdown()
{
	if (UCommonSessionSubsystem* SessionSubsystem = GetSubsystem<UCommonSessionSubsystem>())
	{
		SessionSubsystem->OnPreClientTravelEvent.RemoveAll(this);
	}
	
	Super::Shutdown();
}

void UAuraGameInstance::OnPreClientTravelToSession(FString& URL)
{
}
