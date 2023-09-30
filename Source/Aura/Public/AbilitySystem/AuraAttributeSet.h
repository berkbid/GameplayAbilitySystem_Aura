// Copyright Berkeley Bidwell

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AuraAttributeSet.generated.h"

// Allows ATTRIBUTE_ACCESSORS to define all 4 convenience functions at once
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

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
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UAuraAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ~ Begin UAttributeSet interface
	// Called when any attribute changes, use for clamping only
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	// ~ End UAttributeSet interface
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Health);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxHealth);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, Mana);
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;
	ATTRIBUTE_ACCESSORS(UAuraAttributeSet, MaxMana);
	
protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;
	
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;
	
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;
	
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

protected:
	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Effect")
	//FEffectProperties EffectProperties;

private:
	void FillEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutEffectProperties) const;
};
