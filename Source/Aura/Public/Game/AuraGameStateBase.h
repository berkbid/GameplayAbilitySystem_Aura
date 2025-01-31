// Copyright Berkeley Bidwell

#pragma once

#include "ModularGameState.h"
#include "AuraGameStateBase.generated.h"

class UObject;

/**
 * 
 */
UCLASS(Config = Game)
class AURA_API AAuraGameStateBase : public AModularGameStateBase
{
	GENERATED_BODY()
	
public:

	AAuraGameStateBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void PreInitializeComponents() override;
	virtual void PostInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	//~End of AActor interface

	//~AGameStateBase interface
	virtual void AddPlayerState(APlayerState* PlayerState) override;
	virtual void RemovePlayerState(APlayerState* PlayerState) override;
	virtual void SeamlessTravelTransitionCheckpoint(bool bToTransitionMap) override;
	//~End of AGameStateBase interface
	
};
