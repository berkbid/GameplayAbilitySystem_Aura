// Copyright Berkeley Bidwell

#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExecCalc_Damage)

struct AuraDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
	}
};

static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics DStatics;
	return DStatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef);
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef);
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	//const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	//const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	//const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	//const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	//const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	//const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	// Get source and target tags
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;
	
	// Get damage set by caller magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	// Update the damage based on source and target attributes
	const bool bBlocked = UpdateDamageWithBlock	(Damage, ExecutionParams, EvaluationParameters);
	UpdateDamageWithArmor	(Damage, ExecutionParams, EvaluationParameters);
	const bool bCrit = UpdateDamageWithCritical(Damage, ExecutionParams, EvaluationParameters);

	// Get the Aura gameplay effect context from the Effect Spec and add damage information
	const FGameplayEffectContextHandle& EffectContextHandle = Spec.GetEffectContext();
	// Set block and crit data on context handle
	UAuraAbilitySystemLibrary::SetIsBlockedHit(const_cast<FGameplayEffectContextHandle&>(EffectContextHandle), bBlocked);
	UAuraAbilitySystemLibrary::SetIsCriticalHit(const_cast<FGameplayEffectContextHandle&>(EffectContextHandle), bCrit);
	
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}

bool UExecCalc_Damage::UpdateDamageWithCritical(float& OutDamage, const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluationParameters) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	
	// Critical Hit Chance
	float SourceCriticalHitChance = 0.f;
	const bool bFoundCriticalHitChance = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters, SourceCriticalHitChance);
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance, 0.0f);

	// Critical Hit Resistance
	float TargetCriticalHitResistance = 0.f;
	const bool bFoundCriticalHitResistance = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters, TargetCriticalHitResistance);
	TargetCriticalHitResistance = FMath::Max<float>(TargetCriticalHitResistance, 0.0f);

	// Critical Hit Resistance Coefficient
	float CriticalHitResistanceCoefficient = 0.f;
	GetCalculationCoefficient(CriticalHitResistanceCoefficient, SourceAvatar, FName("CriticalHitResistance"), TargetCombatInterface ? TargetCombatInterface->GetPlayerLevel() : 1);
	
	// Determine if critical hit
	const float CriticalHitRandomFloat = FMath::RandRange(1.f, 100.f);
	const float EffectiveCriticalHitChance = SourceCriticalHitChance - TargetCriticalHitResistance * CriticalHitResistanceCoefficient;
	const bool bCrit = CriticalHitRandomFloat <= EffectiveCriticalHitChance;

	if (bCrit)
	{
		// Critical Hit Damage
		float SourceCriticalHitDamage = 0.f;
		const bool bFoundCriticalHitDamage = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters, SourceCriticalHitDamage);
		SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitDamage, 0.0f);

		OutDamage = OutDamage * 2.f + SourceCriticalHitDamage;
	}
	
	return bCrit;
}

bool UExecCalc_Damage::UpdateDamageWithBlock(float& OutDamage, const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluationParameters) const
{
	// Target block chance
	float TargetBlockChance = 0.f;
	const bool bFoundTargetBlockChance = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters, TargetBlockChance);
	TargetBlockChance = FMath::Max<float>(TargetBlockChance, 0.0f);

	// Determine if block occured and update damage
	const float RandFloat = FMath::RandRange(1.f, 100.f);
	const bool bBlocked = RandFloat <= TargetBlockChance;
	OutDamage = bBlocked ? OutDamage = OutDamage / 2.f : OutDamage;

	return bBlocked;
}

void UExecCalc_Damage::UpdateDamageWithArmor(float& OutDamage, const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluationParameters) const
{
	const UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;
	const ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	const ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);
	
	// Target armor (armor penetration will ignore a percentage of the target's armor)
	float TargetArmor = 0.f;
	const bool bFoundTargetArmor = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters, TargetArmor);
	TargetArmor = FMath::Max<float>(TargetArmor, 0.0f);

	// Source armor penetration
	float SourceArmorPenetration = 0.f;
	const bool bFoundSourceArmorPenetration = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters, SourceArmorPenetration);
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration, 0.0f);

	// Armor Penetration Coefficient
	float ArmorPenetrationCoefficient = 0.f;
	GetCalculationCoefficient(ArmorPenetrationCoefficient, SourceAvatar, FName("ArmorPenetration"), SourceCombatInterface ? SourceCombatInterface->GetPlayerLevel() : 1);

	// Effective Armor Coefficient
	float EffectiveArmorCoefficient = 0.f;
	GetCalculationCoefficient(EffectiveArmorCoefficient, SourceAvatar, FName("EffectiveArmor"), SourceCombatInterface ? SourceCombatInterface->GetPlayerLevel() : 1);
	
	// Armor ignores a percentage of incoming damage
	const float EffectiveArmor = TargetArmor * (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f;
	OutDamage *= (100 - EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
}

void UExecCalc_Damage::GetCalculationCoefficient(float& OutCoefficient, const UObject* WorldContextObject, FName CurveName, int32 PlayerLevel) const
{
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(WorldContextObject);
	const UCurveTable* DamageCalculationCoefficients = CharacterClassInfo ? CharacterClassInfo->DamageCalculationCoefficients : nullptr;
	if (const FRealCurve* Curve = DamageCalculationCoefficients ? DamageCalculationCoefficients->FindCurve(CurveName, FString()) : nullptr)
	{
		OutCoefficient = Curve->Eval(PlayerLevel);
	}
}
