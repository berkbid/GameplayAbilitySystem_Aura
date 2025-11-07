// Copyright Berkeley Bidwell

#include "AbilitySystem/Abilities/AuraGameplayAbility_FireBolt.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraGameplayAbility_FireBolt)

FString UAuraGameplayAbility_FireBolt::GetDescription(int32 Level) const
{
	return GetDescriptionInternal(FString("FIRE BOLT:"), Level);
}

FString UAuraGameplayAbility_FireBolt::GetNextLevelDescription(int32 Level) const 
{
	return GetDescriptionInternal(FString("NEXT LEVEL:"), Level);
}

FString UAuraGameplayAbility_FireBolt::GetDescriptionInternal(const FString& InTitle, int32 Level) const
{
	float FireDamage = Damage.GetValueAtLevel(Level);
	const float ManaCost = GetManaCost(Level);
	const int32 NumFireBolts = FMath::Min(Level, NumProjectiles);
	const float Cooldown = GetCooldown(Level);
	
	return FString::Printf(TEXT(
		// Title and level
		"<Title>%s %d</>\n\n"
		// Level
		"<Small>Level: </><Level>%d</>\n"
		// Mana cost
		"<Small>Mana Cost: </><ManaCost>%.1f</>\n"
		// Cooldown
		"<Small>Cooldown: </><Cooldown>%.1f</>\n\n"
		// Num projectiles
		"Launches %d bolt%s of fire, "
		// Damage
		"exploding on impact an dealing: <Damage>%.1f</>"
		" fire damage with a chance to burn\n\n"),
		*InTitle, Level, Level, ManaCost, Cooldown, NumFireBolts, NumFireBolts > 1 ? TEXT("s") : TEXT(""), FireDamage);
}
