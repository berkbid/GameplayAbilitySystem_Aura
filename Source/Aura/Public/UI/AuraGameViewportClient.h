// Copyright Berkeley Bidwell

#pragma once

#include "CommonGameViewportClient.h"
#include "AuraGameViewportClient.generated.h"

class UGameInstance;
class UObject;

/**
 * 
 */
UCLASS(BlueprintType)
class AURA_API UAuraGameViewportClient : public UCommonGameViewportClient
{
	GENERATED_BODY()
	
public:
	UAuraGameViewportClient();

	virtual void Init(struct FWorldContext& WorldContext, UGameInstance* OwningGameInstance, bool bCreateNewAudioDevice = true) override;
	
};
