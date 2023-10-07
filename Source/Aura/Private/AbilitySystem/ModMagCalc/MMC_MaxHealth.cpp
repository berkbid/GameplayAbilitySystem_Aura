// Copyright Berkeley Bidwell

#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// Snapshot captures as soon as the effect spec is created
	VigorDef.bSnapshot = false; // Capture when gameplay effect spec is created or applied?

	// Need to add to array of relevant attributes for calculation
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Want max health dependent on vigor and player level
	// Spec = gameplay effect spec that has a modifier using this calculation
	
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Get value of Vigor attribute on Target
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	
	// Don't let vigor be below 0
	Vigor = FMath::Max<float>(Vigor, 0.f);
	
	int32 PlayerLevel = 1;
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject()))
	{
		PlayerLevel = CombatInterface->GetPlayerLevel();
	}
	UE_LOG(LogTemp, Warning, TEXT("Recalculating Max Health based on vigor and player level"));
	return 80.f + Vigor * 2.5f + 10.f * PlayerLevel;
}
