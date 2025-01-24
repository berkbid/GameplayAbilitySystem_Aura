// Copyright Berkeley Bidwell

#pragma once

#include "CommonGameInstance.h"
#include "AuraGameInstance.generated.h"

class UObject;

/**
 * UAuraGameInstance
 */
UCLASS(Config = Game)
class AURA_API UAuraGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
	
public:
	UAuraGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	virtual void Init() override;
	virtual void Shutdown() override;

	void OnPreClientTravelToSession(FString& URL);
};
