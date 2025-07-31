// Copyright Berkeley Bidwell

#include "AbilitySystem/Data/AbilityInfo.h"
#include "AuraLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AbilityInfo)

FAuraAbilityInfo UAbilityInfo::FindAbilityInfo(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	const FAuraAbilityInfo* FoundAbilityInfo = AbilityInformation.FindByPredicate([AbilityTag](const FAuraAbilityInfo& AbilityInfo)
	{
		return AbilityInfo.AbilityTag.MatchesTagExact(AbilityTag);
	});

	if (bLogNotFound && !FoundAbilityInfo)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find info for AbilityTag [%s] on AbilityInfo [%s]"), *AbilityTag.ToString(), *GetNameSafe(this));
	}
	
	return FoundAbilityInfo ? *FoundAbilityInfo : FAuraAbilityInfo();
}
