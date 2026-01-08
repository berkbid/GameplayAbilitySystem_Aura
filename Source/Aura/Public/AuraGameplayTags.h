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
	
	// Meta attributes
	FGameplayTag Attributes_Meta_IncomingXp;
	
	/*
	 * INPUT TAGS
	 */
	FGameplayTag InputTag_LMB;
	FGameplayTag InputTag_RMB;
	FGameplayTag InputTag_1;
	FGameplayTag InputTag_2;
	FGameplayTag InputTag_3;
	FGameplayTag InputTag_4;
	FGameplayTag InputTag_Passive_1;
	FGameplayTag InputTag_Passive_2;
	
	// Tag for GE set by caller
	FGameplayTag Damage;
	FGameplayTag Damage_Fire;
	FGameplayTag Damage_Lightning;
	FGameplayTag Damage_Arcane;
	FGameplayTag Damage_Physical;
	
	// Secondary attributes treated as resistances
	FGameplayTag Attributes_Resistance_Fire;
	FGameplayTag Attributes_Resistance_Lightning;
	FGameplayTag Attributes_Resistance_Arcane;
	FGameplayTag Attributes_Resistance_Physical;

	// Debuffs
	/** Debuff associated with fire damage type */
	FGameplayTag Debuff_Burn;
	/** Debuff associated with lightning damage type */
	FGameplayTag Debuff_Stun;
	/** Debuff associated with arcane damage type */
	FGameplayTag Debuff_Arcane;
	/** Debuff associated with physical damage type */
	FGameplayTag Debuff_Physical;

	FGameplayTag Debuff_Info_Chance;
	FGameplayTag Debuff_Info_Damage;
	FGameplayTag Debuff_Info_Frequency;
	FGameplayTag Debuff_Info_Duration;
	
	// Abilities
	FGameplayTag Abilities_None;
	FGameplayTag Abilities_Attack;
	FGameplayTag Abilities_Summon;
	FGameplayTag Abilities_Fire_FireBolt;
	FGameplayTag Abilities_Lightning_Electrocute;
	FGameplayTag Abilities_HitReact;
	
	// Ability status
	FGameplayTag Abilities_Status_Locked;
	FGameplayTag Abilities_Status_Eligible;
	FGameplayTag Abilities_Status_Unlocked;
	FGameplayTag Abilities_Status_Equipped;
	
	// Ability type
	FGameplayTag Abilities_Type_Offensive;
	FGameplayTag Abilities_Type_Passive;
	FGameplayTag Abilities_Type_None;
	
	// Cooldowns
	FGameplayTag Cooldown_Fire_FireBolt;
	FGameplayTag Cooldown_Lightning_Electrocute;
	
	/** Mapping of damage type gameplay tags to resistance type gameplay tags */
	TMap<FGameplayTag, FGameplayTag> DamageTypesToResistances;
	TMap<FGameplayTag, FGameplayTag> DamageTypesToDebuffs;
	
	// Tag to indicate reacting to a hit
	FGameplayTag Effects_HitReact;

	FGameplayTag CombatSocket_Weapon;
	FGameplayTag CombatSocket_RightHand;
	FGameplayTag CombatSocket_LeftHand;
	FGameplayTag CombatSocket_Tail;

	FGameplayTag Montage_Attack_1;
	FGameplayTag Montage_Attack_2;
	FGameplayTag Montage_Attack_3;
	FGameplayTag Montage_Attack_4;
	
	FGameplayTag Player_Block_InputPressed;
	FGameplayTag Player_Block_InputReleased;
	FGameplayTag Player_Block_InputHeld;
	FGameplayTag Player_Block_CursorTrace;
	
	FGameplayTag Player_Status_Dead;
	
protected:

private:
    static FAuraGameplayTags GameplayTags;

	bool bInitialized = false;
};