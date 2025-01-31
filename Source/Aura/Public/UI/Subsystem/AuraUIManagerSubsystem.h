// Copyright Berkeley Bidwell

#pragma once

#include "GameUIManagerSubsystem.h"
#include "AuraUIManagerSubsystem.generated.h"

class FSubsystemCollectionBase;
class UObject;

/**
 * 
 */
UCLASS()
class AURA_API UAuraUIManagerSubsystem : public UGameUIManagerSubsystem
{
	GENERATED_BODY()
	
public:

	UAuraUIManagerSubsystem();

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
};
