// Copyright Berkeley Bidwell

#include "AbilitySystem/Data/CharacterClassInfo.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CharacterClassInfo)

FCharacterClassDefaultInfo UCharacterClassInfo::GetClassDefaultInfo(ECharacterClass CharacterClass) const
{
	return CharacterClassInformation.FindChecked(CharacterClass);
}
