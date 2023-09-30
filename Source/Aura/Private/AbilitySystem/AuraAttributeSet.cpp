// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(100.f);
}

void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Registers attribute to be replicated with no conditions, always set on client even if value doesn't change when set (may want to respond to act of setting)
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana,			COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana,		COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Health pre-attribute change: %f"), NewValue);
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	else if (Attribute == GetMaxHealthAttribute())
	{
		//UE_LOG(LogTemp, Warning, TEXT("MaxHealth pre-attribute change: %f"), NewValue);
	}
	else if (Attribute == GetManaAttribute())
	{
		//UE_LOG(LogTemp, Warning, TEXT("Mana pre-attribute change: %f"), NewValue);
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
	else if (Attribute == GetMaxManaAttribute())
	{
		//UE_LOG(LogTemp, Warning, TEXT("MaxMana pre-attribute change: %f"), NewValue);
	}
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	// Access to lots of data post gameplay effect execute
	FEffectProperties Props(Data);
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
					OutEffectProperties.SourceController = Cast<APlayerController>(Pawn->GetController());
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

void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// Inform ability system of new replicated value
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
	//UE_LOG(LogTemp, Warning, TEXT("OnRep_Health - Old: %f, New: %f"), OldHealth.GetCurrentValue(), Health.GetCurrentValue());
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
	
	if (SourceASC)
	{
		if (SourceASC->AbilityActorInfo.IsValid() && SourceASC->AbilityActorInfo->AvatarActor.IsValid())
		{
			// SOURCE INFO
			SourceAvatarActor = SourceASC->AbilityActorInfo->AvatarActor.Get();
			SourceController = SourceASC->AbilityActorInfo->PlayerController.IsValid() ? SourceASC->AbilityActorInfo->PlayerController.Get() : nullptr;
			
			// Fall back to trying to get controller from pawn if invalid
			if (!SourceController)
			{
				if (const APawn* Pawn = Cast<APawn>(SourceAvatarActor))
				{
					SourceController = Cast<APlayerController>(Pawn->GetController());
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
