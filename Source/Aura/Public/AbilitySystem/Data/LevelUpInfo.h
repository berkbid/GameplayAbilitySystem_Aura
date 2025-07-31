// Copyright Berkeley Bidwell

#pragma once

#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

class UObject;

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 XpRequirement = 0;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 AttributeGain = 5;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 SpellPointGain = 1;
};

/**
 * Data asset for leveling up information.
 */
UCLASS(Const)
class ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	int32 FindLevelForXp(int32 Xp) const;
	float FindXpPercentForXp(int32 Xp) const;

	int32 GetAttributeGainForLevel(int32 Level) const;
	int32 GetSpellPointGainForLevel(int32 Level) const;
	
	//~UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	
	//~End of UObject interface
	// Array index represents Level - 1. Ex. Index 0 is for level 1 information
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUpInformation")
	TArray<FAuraLevelUpInfo> LevelUpInformation;
};
