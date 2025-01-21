// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "GameFramework/Controller.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAttributeSet)

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute());
	
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMana, GetMaxManaAttribute());
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Registers attribute to be replicated with no conditions, always set on client even if value doesn't change when set (may want to respond to act of setting)

	// Primary Attributes
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Strength,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Intelligence,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Resilience,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Vigor,		COND_None, REPNOTIFY_Always);

	// Secondary Attributes
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Armor,				COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArmorPenetration,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, BlockChance,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitChance,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitDamage,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, CriticalHitResistance,COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, HealthRegeneration,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ManaRegeneration,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana,				COND_None, REPNOTIFY_Always);
	
	// Vital Attributes
	
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana,		COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Not clamping here anymore, seems to be problematic apparently and has no use
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Access to lots of data post gameplay effect execute
	FEffectProperties Props(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// GetHealth() returns clamped value in PreAttributeChange but not actual value, so we cannot test if it is in range, need to call SetHealth always
		const float ClampedHealth = FMath::Clamp(GetHealth(), 0.f, GetMaxHealth());
		//UE_LOG(LogTemp, Warning, TEXT("Post GE health value: %f, setting health clamped: %f"), GetHealth(), ClampedHealth);
		SetHealth(ClampedHealth);
	}
	else if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		const float ClampedMana = FMath::Clamp(GetMana(), 0.f, GetMaxMana());
		//UE_LOG(LogTemp, Warning, TEXT("Post GE Mana value: %f, setting Mana clamped: %f"), GetMana(), ClampedMana);
		SetMana(ClampedMana);
	}
}

UAuraAbilitySystemComponent* UAuraAttributeSet::GetAuraAbilitySystemComponent() const
{
	return Cast<UAuraAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void UAuraAttributeSet::FillEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutEffectProperties) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this attribute set)
	 
	OutEffectProperties.EffectContextHandle =  Data.EffectSpec.GetContext();
	OutEffectProperties.SourceASC = OutEffectProperties.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	
	if (OutEffectProperties.SourceASC)
	{
		if (OutEffectProperties.SourceASC->AbilityActorInfo.IsValid() && OutEffectProperties.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
		{
			// SOURCE INFO
			OutEffectProperties.SourceAvatarActor = OutEffectProperties.SourceASC->AbilityActorInfo->AvatarActor.Get();
			OutEffectProperties.SourceController = OutEffectProperties.SourceASC->AbilityActorInfo->PlayerController.IsValid() ? OutEffectProperties.SourceASC->AbilityActorInfo->PlayerController.Get() : nullptr;
			
			// Fall back to trying to get controller from pawn if invalid
			if (!OutEffectProperties.SourceController)
			{
				if (const APawn* Pawn = Cast<APawn>(OutEffectProperties.SourceAvatarActor))
				{
					OutEffectProperties.SourceController = Pawn->GetController();
				}
			}
			
			// Get source character
			if (OutEffectProperties.SourceController)
			{
				OutEffectProperties.SourceCharacter = Cast<ACharacter>(OutEffectProperties.SourceController->GetPawn());
			}
			//OutEffectProperties.SourceCharacter = Cast<ACharacter>(OutEffectProperties.SourceAvatarActor);
		}
	}
	
	// TARGET INFO
		
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		OutEffectProperties.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		OutEffectProperties.TargetController = Data.Target.AbilityActorInfo->PlayerController.IsValid() ? Data.Target.AbilityActorInfo->PlayerController.Get(): nullptr;
		OutEffectProperties.TargetCharacter = Cast<ACharacter>(OutEffectProperties.TargetAvatarActor);
		//OutEffectProperties.TargetASC = &Data.Target;
		//OutEffectProperties.TargetASC = GetOwningAbilitySystemComponent();
		OutEffectProperties.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OutEffectProperties.TargetAvatarActor);
	}
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);
}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

void UAuraAttributeSet::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Armor, OldArmor);
}

void UAuraAttributeSet::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArmorPenetration, OldArmorPenetration);
}

void UAuraAttributeSet::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, BlockChance, OldBlockChance);
}

void UAuraAttributeSet::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitChance, OldCriticalHitChance);
}

void UAuraAttributeSet::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitDamage, OldCriticalHitDamage);
}

void UAuraAttributeSet::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, CriticalHitResistance, OldCriticalHitResistance);
}

void UAuraAttributeSet::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, HealthRegeneration, OldHealthRegeneration);
}

void UAuraAttributeSet::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ManaRegeneration, OldManaRegeneration);
}

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// Inform ability system of new replicated value
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Health - Old: %f, New: %f"), OldHealth.GetCurrentValue(), Health.GetCurrentValue());
}

void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_MaxHealth - Old: %f, New: %f"), OldMaxHealth.GetCurrentValue(), MaxHealth.GetCurrentValue());
}

void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_Mana - Old: %f, New: %f"), OldMana.GetCurrentValue(), Mana.GetCurrentValue());
}

void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_MaxMana - Old: %f, New: %f"), OldMaxMana.GetCurrentValue(), MaxMana.GetCurrentValue());
}

FEffectProperties::FEffectProperties(const FGameplayEffectModCallbackData& Data)
{
	// Source = causer of the effect, Target = target of the effect (owner of this attribute set)
	 
	EffectContextHandle =  Data.EffectSpec.GetContext();
	SourceASC = EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();
	
	// SOURCE INFO
	
	if (SourceASC)
	{
		if (SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
		{
			
			SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
			SourceController = SourceASC->AbilityActorInfo->PlayerController.IsValid() ? SourceASC->AbilityActorInfo->PlayerController.Get() : nullptr;
			
			// Fall back to trying to get controller from pawn if invalid
			if (!SourceController)
			{
				if (const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
				{
					SourceController = Pawn->GetController();
				}
			}
			
			// Get source character
			if (SourceController)
			{
				SourceCharacter = Cast<ACharacter>(SourceController->GetPawn());
			}
			//SourceCharacter = Cast<ACharacter>(SourceAvatarActor);
		}
	}
	
	// TARGET INFO
		
	if (Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		TargetController = Data.Target.AbilityActorInfo->PlayerController.IsValid() ? Data.Target.AbilityActorInfo->PlayerController.Get(): nullptr;
		TargetCharacter = Cast<ACharacter>(TargetAvatarActor);
		//TargetASC = &Data.Target;
		//TargetASC = GetOwningAbilitySystemComponent();
		TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetAvatarActor);
	}
}
