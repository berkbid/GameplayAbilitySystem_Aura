// Copyright Berkeley Bidwell

#include "AuraAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAssetManager)

UAuraAssetManager::UAuraAssetManager()
{
}

UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	return *Cast<UAuraAssetManager>(GEngine->AssetManager);
}

void UAuraAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	// Instead of this, the FAuraGameplayTags::Get() will call this function the first time it is called
	//FAuraGameplayTags::InitializeNativeGameplayTags();

	// Necessary to use target data in ability tasks (No longer necessary in 5.3+)
	//UAbilitySystemGlobals::Get().InitGlobalData();
}
