// Copyright Berkeley Bidwell

#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

class AActor;
class AController;
class ACharacter;
class UAuraAbilitySystemComponent;
class UObject;
struct FGameplayEffectContextHandle;
struct FGameplayEffectModCallbackData;
struct FGameplayTag;

/**
 * This macro defines a set of helper functions for accessing and initializing attributes.
 *
 * The following example of the macro:
 *		ATTRIBUTE_ACCESSORS(ULyraHealthSet, Health)
 * will create the following functions:
 *		static FGameplayAttribute GetHealthAttribute();
 *		float GetHealth() const;
 *		void SetHealth(float NewVal);
 *		void InitHealth(float NewVal);
 */
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

// Delegate that can have a function bound to it, returns value of type FGameplayAttribute
//DECLARE_DELEGATE_RetVal(FGameplayAttribute, FAttributeSignature);

USTRUCT(BlueprintType)
struct FEffectProperties
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<UAbilitySystemComponent> SourceASC = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<AActor> SourceAvatarActor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<AController> SourceController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<ACharacter> SourceCharacter = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<UAbilitySystemComponent> TargetASC = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<AActor> TargetAvatarActor = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<AController> TargetController = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Source")
	TObjectPtr<ACharacter> TargetCharacter = nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	FGameplayEffectContextHandle EffectContextHandle;

	FEffectProperties() {}
	
	FEffectProperties(UAbilitySystemComponent* InSourceASC, AActor* InSourceAvatarActor, AController* InSourceController, ACharacter* InSourceCharacter,
		UAbilitySystemComponent* InTargetASC, AActor* InTargetAvatarActor, AController* InTargetController, ACharacter* InTargetCharacter, const FGameplayEffectContextHandle& InEffectContextHandle)
	: SourceASC(InSourceASC), SourceAvatarActor(InSourceAvatarActor), SourceController(InSourceController), SourceCharacter(InSourceCharacter),
	TargetASC(InTargetASC), TargetAvatarActor(InTargetAvatarActor), TargetController(InTargetController), TargetCharacter(InTargetCharacter), EffectContextHandle(InEffectContextHandle) {}

	FEffectProperties(const FGameplayEffectModCallbackData& Data);
};

/**
 * 
 */
UCLASS(MinimalAPI)
class UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	AURA_API UAuraAttributeSet();

	AURA_API virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ~ Begin UAttributeSet interface
	// Called when any attribute changes, use for clamping only
	AURA_API virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	AURA_API virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
	AURA_API virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	// ~ End UAttributeSet interface
	
	AURA_API UAuraAbilitySystemComponent* GetAuraAbilitySystemComponent() const;

	/** Call after increasing level for player in player state. This will maximize health and mana. */
	AURA_API void LevelUp();

public:
	/** Map of gameplay tag to the gameplay attribute */
	TMap<FGameplayTag, FGameplayAttribute> TagsToAttributes;

	/**
	 * Given an attribute tag, return true if it is valid to increment by Points amount.
	 * Mostly for negative points amount, checking that we don't go below base attribute value.
	 */
	AURA_API bool CanApplyAttributePoints(const FGameplayTag& AttributeTag, int32 Points) const;
	
	/*
	 * Primary Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Strength, Category = "Primary Attributes")
	FGameplayAttributeData Strength;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Strength);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Intelligence, Category = "Primary Attributes")
	FGameplayAttributeData Intelligence;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Intelligence);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Resilience, Category = "Primary Attributes")
	FGameplayAttributeData Resilience;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Resilience);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Vigor, Category = "Primary Attributes")
	FGameplayAttributeData Vigor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Vigor);

	/*
	 * Secondary Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Armor, Category = "Secondary Attributes")
	FGameplayAttributeData Armor;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Armor);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ArmorPenetration, Category = "Secondary Attributes")
	FGameplayAttributeData ArmorPenetration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArmorPenetration);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_BlockChance, Category = "Secondary Attributes")
	FGameplayAttributeData BlockChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, BlockChance);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalHitChance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitChance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitChance);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalHitDamage, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitDamage);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CriticalHitResistance, Category = "Secondary Attributes")
	FGameplayAttributeData CriticalHitResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, CriticalHitResistance);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_HealthRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData HealthRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, HealthRegeneration);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ManaRegeneration, Category = "Secondary Attributes")
	FGameplayAttributeData ManaRegeneration;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ManaRegeneration);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category = "Secondary Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, Category = "Secondary Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);

	/*
	 * Resistance Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_FireResistance, Category = "Resistance Attributes")
	FGameplayAttributeData FireResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, FireResistance);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_LightningResistance, Category = "Resistance Attributes")
	FGameplayAttributeData LightningResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, LightningResistance);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_ArcaneResistance, Category = "Resistance Attributes")
	FGameplayAttributeData ArcaneResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, ArcaneResistance);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_PhysicalResistance, Category = "Resistance Attributes")
	FGameplayAttributeData PhysicalResistance;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, PhysicalResistance);
	
	/*
	 * Vital Attributes
	 */
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category = "Vital Attributes", meta = (HideFromModifiers))
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);

	/*
	 * Meta Attributes (non-replicated)
	 */
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingDamage;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingDamage);
	
	UPROPERTY(BlueprintReadOnly, Category = "Meta Attributes")
	FGameplayAttributeData IncomingXp;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, IncomingXp);
	
protected:
	/*
	 * Primary Attributes
	 */
	UFUNCTION()
	AURA_API void OnRep_Strength(const FGameplayAttributeData& OldStrength) const;
	UFUNCTION()
	AURA_API void OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const;
	UFUNCTION()
	AURA_API void OnRep_Resilience(const FGameplayAttributeData& OldResilience) const;
	UFUNCTION()
	AURA_API void OnRep_Vigor(const FGameplayAttributeData& OldVigor) const;

	/*
	 * Secondary Attributes
	 */
	UFUNCTION()
	AURA_API void OnRep_Armor(const FGameplayAttributeData& OldArmor) const;
	UFUNCTION()
	AURA_API void OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const;
	UFUNCTION()
    AURA_API void OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const;
	UFUNCTION()
	AURA_API void OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const;
	UFUNCTION()
	AURA_API void OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const;
	UFUNCTION()
	AURA_API void OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const;
	UFUNCTION()
	AURA_API void OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const;
	UFUNCTION()
	AURA_API void OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const;
	UFUNCTION()
	AURA_API void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	UFUNCTION()
	AURA_API void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

	/*
	 * Resistance Attributes
	 */
	UFUNCTION()
	AURA_API void OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const;
	UFUNCTION()
	void OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const;
	UFUNCTION()
	AURA_API void OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const;
	UFUNCTION()
	AURA_API void OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const;
	
	/*
	 * Vital Attributes
	 */
	UFUNCTION()
	AURA_API void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	UFUNCTION()
	AURA_API void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	
protected:


private:
	void HandleIncomingDamage(const FEffectProperties& Props, const bool bDotDamage);
	void HandleIncomingXP(const FEffectProperties& Props);
	static void Debuff(const FEffectProperties& Props);
	void FillEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutEffectProperties) const;
	static void ShowFloatingText(const FEffectProperties& EffectProperties, float Damage, bool bBlockedHit, bool bCriticalHit);
	static void SendXpEvent(const FEffectProperties& Props);
	
private:

};
