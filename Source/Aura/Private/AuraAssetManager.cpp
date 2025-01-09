// Copyright Berkeley Bidwell

#include "AuraAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	return *Cast<UAuraAssetManager>(GEngine->AssetManager);
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	FAuraGameplayTags::InitializeNativeGameplayTags();

	// Necessary to use target data in ability tasks (No longer necessary in 5.3+)
	//UAbilitySystemGlobals::Get().InitGlobalData();
}
