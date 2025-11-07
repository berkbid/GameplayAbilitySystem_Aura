// Copyright Berkeley Bidwell

#pragma once

#include "GameplayEffectExecutionCalculation.h"
#include "ExecCalc_Damage.generated.h"

class UObject;
struct FGameplayEffectSpec;
struct FGameplayEffectCustomExecutionParameters;
struct FAggregatorEvaluateParameters;

/**
 * 
 */
UCLASS()
class UExecCalc_Damage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()
	
public:
	UExecCalc_Damage();

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;

protected:
	void CalculateDebuffEffects(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParameters) const;

	float CalculateDamageWithResistances(const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FGameplayEffectSpec& Spec, const FAggregatorEvaluateParameters& EvaluationParameters) const;

private:
	bool UpdateDamageWithCritical(float& OutDamage, const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluationParameters) const;
	bool UpdateDamageWithBlock(float& OutDamage, const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluationParameters) const;
	void UpdateDamageWithArmor(float& OutDamage, const FGameplayEffectCustomExecutionParameters& ExecutionParams, const FAggregatorEvaluateParameters& EvaluationParameters) const;

	void GetCalculationCoefficient(float& OutCoefficient, const UObject* WorldContextObject , FName CurveName, int32 PlayerLevel) const;
};
