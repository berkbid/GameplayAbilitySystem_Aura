// Copyright Berkeley Bidwell

#include "UI/AuraGameViewportClient.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameViewportClient)

class UGameInstance;

UAuraGameViewportClient::UAuraGameViewportClient()
	: Super(FObjectInitializer::Get())
{
}

void UAuraGameViewportClient::Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance,
	bool bCreateNewAudioDevice)
{
	Super::Init(WorldContext, OwningGameInstance, bCreateNewAudioDevice);
	
	//const bool UseHardwareCursor = ICommonUIModule::GetSettings().GetPlatformTraits().HasTag(GameViewportTags::TAG_Platform_Trait_Input_HardwareCursor);
	//SetUseSoftwareCursorWidgets(!UseHardwareCursor);
}
