// Copyright Berkeley Bidwell

#include "AbilitySystem/Data/LevelUpInfo.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(LevelUpInfo)

#define LOCTEXT_NAMESPACE "AuraSystem"

int32 ULevelUpInfo::FindLevelForXp(int32 Xp) const
{
	int32 Level = 1;

	// Index 0 = level 1
	for (const FAuraLevelUpInfo& LevelUpInfo : LevelUpInformation)
	{
		if (LevelUpInfo.XpRequirement > Xp)
		{
			return Level;
		}
		Level++;
	}
	
	return Level;
}

float ULevelUpInfo::FindXpPercentForXp(int32 Xp) const
{
	int32 PreviousXpRequirement = 0;

	for (const FAuraLevelUpInfo& LevelUpInfo : LevelUpInformation)
	{
		const int32 NextLvlUpRequirement = LevelUpInfo.XpRequirement;

		// Using > means we should level up with 100% xp percent
		if (NextLvlUpRequirement > Xp)
		{
			return static_cast<float>(Xp - PreviousXpRequirement) / static_cast<float>(NextLvlUpRequirement - PreviousXpRequirement);
		}
		PreviousXpRequirement = NextLvlUpRequirement;
	}
	
	return 0.0f;
}

int32 ULevelUpInfo::GetAttributeGainForLevel(int32 Level) const
{
	if (LevelUpInformation.IsValidIndex(Level - 1))
	{
		return LevelUpInformation[Level - 1].AttributeGain;
	}
	return 0;
}

int32 ULevelUpInfo::GetSpellPointGainForLevel(int32 Level) const
{
	if (LevelUpInformation.IsValidIndex(Level - 1))
	{
		return LevelUpInformation[Level - 1].SpellPointGain;
	}
	return 0;
}

#if WITH_EDITOR
EDataValidationResult ULevelUpInfo::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 PreviousXp = 0;
	int32 LevelUpInfoIndex = 0;
	for (const FAuraLevelUpInfo& LevelUpInfo : LevelUpInformation)
	{
		if (LevelUpInfo.XpRequirement <= PreviousXp)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("LevelUpInformation", "Invalid XpRequirement in index {0}"), FText::AsNumber(LevelUpInfoIndex)));
			break;
		}
		PreviousXp = LevelUpInfo.XpRequirement;
		LevelUpInfoIndex++;
	}
	
	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
