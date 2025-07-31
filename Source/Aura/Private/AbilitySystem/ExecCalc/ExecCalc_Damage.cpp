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
	
	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag, FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs;
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false);
		
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false);
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false);

		// Create mapping of gameplay tag to the attribute capture definition
		const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		
		TagsToCaptureDefs.Empty();
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(AuraGameplayTags.Attributes_Resistance_Physical, PhysicalResistanceDef);
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
	
	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef);
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef);
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

	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& DamageToResistanceTagPair : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		// Get the damage for this damage type
		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageToResistanceTagPair.Key, false);
		if (DamageTypeValue <= 0.f)
		{
			continue;
		}
		
		const FGameplayTag ResistanceTypeTag = DamageToResistanceTagPair.Value;
		checkf(DamageStatics().TagsToCaptureDefs.Contains(ResistanceTypeTag), TEXT("Missing resistance type tag %s in tags to capture defs mapping in ExecCalc_Damage!"), *ResistanceTypeTag.ToString());
		const FGameplayEffectAttributeCaptureDefinition ResistanceCaptureDef = DamageStatics().TagsToCaptureDefs[ResistanceTypeTag];
		
		float TargetResistance = 0.f;
		const bool bFoundTargetResistance = ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(ResistanceCaptureDef, EvaluationParameters, TargetResistance);
		TargetResistance = FMath::Clamp(TargetResistance, 0.f, 100.f);

		// Update damage based on targets resistance to this damage type
		DamageTypeValue *= (100.f - TargetResistance) / 100.f;
		
		Damage += DamageTypeValue;
	}
	
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
	const int32 TargetPlayerLevel = TargetAvatar && TargetAvatar->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetPlayerLevel(TargetAvatar) : 1;
	
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
	GetCalculationCoefficient(CriticalHitResistanceCoefficient, SourceAvatar, FName("CriticalHitResistance"), TargetPlayerLevel);
	
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
	const int32 SourcePlayerLevel = SourceAvatar && SourceAvatar->Implements<UCombatInterface>() ? ICombatInterface::Execute_GetPlayerLevel(SourceAvatar) : 1;

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
	GetCalculationCoefficient(ArmorPenetrationCoefficient, SourceAvatar, FName("ArmorPenetration"), SourcePlayerLevel);

	// Effective Armor Coefficient
	float EffectiveArmorCoefficient = 0.f;
	GetCalculationCoefficient(EffectiveArmorCoefficient, SourceAvatar, FName("EffectiveArmor"), SourcePlayerLevel);
	
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
