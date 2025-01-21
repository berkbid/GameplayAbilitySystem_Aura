// Copyright Berkeley Bidwell

#pragma once

#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * UAuraAssetManager
 *
 *	Game implementation of the asset manager that overrides functionality and stores game-specific types.
 *	It is expected that most games will want to override AssetManager as it provides a good place for game-specific loading logic.
 *	This class is used by setting 'AssetManagerClassName' in DefaultEngine.ini.
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:
	UAuraAssetManager();
	
	static UAuraAssetManager& Get();

protected:
	virtual void StartInitialLoading() override;
	
};
