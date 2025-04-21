// Copyright Berkeley Bidwell

#pragma once

#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * Singleton containing native Gameplay Tags
 */
struct FAuraGameplayTags
{
 
public:
    static const FAuraGameplayTags& Get()
    {
    	if (!GameplayTags.bInitialized)
    	{
    		InitializeNativeGameplayTags();
    	}
	    return GameplayTags;
    }
    static void InitializeNativeGameplayTags();

	/*
	 * PRIMARY ATTRIBUTE TAGS
	 */
	FGameplayTag Attributes_Primary_Strength;
	FGameplayTag Attributes_Primary_Intelligence;
	FGameplayTag Attributes_Primary_Resilience;
	FGameplayTag Attributes_Primary_Vigor;
	
	/*
	 * SECONDARY ATTRIBUTE TAGS
	 */
    FGameplayTag Attributes_Secondary_Armor;
    FGameplayTag Attributes_Secondary_ArmorPenetration;
    FGameplayTag Attributes_Secondary_BlockChance;
    FGameplayTag Attributes_Secondary_CriticalHitChance;
    FGameplayTag Attributes_Secondary_CriticalHitDamage;
    FGameplayTag Attributes_Secondary_CriticalHitResistance;
    FGameplayTag Attributes_Secondary_HealthRegeneration;
    FGameplayTag Attributes_Secondary_ManaRegeneration;
	FGameplayTag Attributes_Secondary_MaxHealth;
    FGameplayTag Attributes_Secondary_MaxMana;

	// Secondary attributes treated as resistances
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;
	
	/*
	 * INPUT TAGS
	 */
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	
	// Tag for GE set by caller
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;

	/** Mapping of damage type gameplay tags to resistance type gameplay tags */
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	
	// Tag to indicate reacting to a hit
	FGameplayTag Effects_HitReact;
	
protected:

private:
    static FAuraGameplayTags GameplayTags;

	bool bInitialized = false;
};