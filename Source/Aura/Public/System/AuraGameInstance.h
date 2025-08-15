// Copyright Berkeley Bidwell

#pragma once

#include "CommonGameInstance.h"
#include "AuraGameInstance.generated.h"

class UObject;
class UCharacterClassInfo;
class UAbilityInfo;

/**
 * UAuraGameInstance
 */
UCLASS(MinimalAPI, Config = Game)
class UAuraGameInstance : public UCommonGameInstance
{
	GENERATED_BODY()
	
public:
	AURA_API UAuraGameInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
protected:
	AURA_API virtual void Init() override;
	AURA_API virtual void Shutdown() override;

	AURA_API void OnPreClientTravelToSession(FString& URL);

public:
	// TODO: Can this be on game mode instead? Does client ever need this data?
	UPROPERTY(EditDefaultsOnly, Category = "Character Class Defaults")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	UPROPERTY(EditDefaultsOnly, Category = "Ability Info")
	TObjectPtr<UAbilityInfo> AbilityInfo;
};
