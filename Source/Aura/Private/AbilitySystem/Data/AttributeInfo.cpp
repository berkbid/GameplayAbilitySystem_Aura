// Copyright Berkeley Bidwell

#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AttributeInfo)

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	if (const FAuraAttributeInfo* FoundInfo = AttributeInformation.FindByPredicate([AttributeTag](const FAuraAttributeInfo& Info)
		{ return Info.AttributeTag.MatchesTagExact(AttributeTag); }))
	{
		return *FoundInfo;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogAura, Error, TEXT("Can't find info for attribute tag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(), *GetNameSafe(this));
	}
	
	return FAuraAttributeInfo();
}
