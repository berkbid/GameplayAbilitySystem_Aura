// Copyright Berkeley Bidwell

#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	if (const FAuraAttributeInfo* FoundInfo = AttributeInformation.FindByPredicate([AttributeTag](const FAuraAttributeInfo& Info)
		{ return Info.AttributeTag.MatchesTagExact(AttributeTag); }))
	{
		return *FoundInfo;
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find info for attribute tag [%s] on AttributeInfo [%s]."), *AttributeTag.ToString(), *GetNameSafe(this));
	}
	
	return  FAuraAttributeInfo();
}
