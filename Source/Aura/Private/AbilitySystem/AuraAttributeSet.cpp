// Copyright Berkeley Bidwell

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AuraAbilityTypes.h"
#include "AuraGameplayTags.h"
#include "AuraLogChannels.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"
#include "Interaction/CombatInterface.h"
#include "Interaction/PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Player/AuraPlayerController.h"
#include "GameplayEffectTypes.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraAttributeSet)

UAuraAttributeSet::UAuraAttributeSet()
{
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
	// Primary Attributes
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute());
	
	// Secondary Attributes
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

	// Resistance Attributes
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute());
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute());
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

	// Resistance Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, FireResistance,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, LightningResistance,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, ArcaneResistance,		COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, PhysicalResistance,	COND_None, REPNOTIFY_Always);
	
	// Vital Attributes
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health,	COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana,		COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// Not clamping here anymore, seems problematic apparently and has no use
}

void UAuraAttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
}

void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	// Only server is in here
	
	Super::PostGameplayEffectExecute(Data);

	// Access to lots of data post gameplay effect execute
	FEffectProperties Props(Data);

	// Ensure target character implements combat interface and is not dead
	if (!Props.TargetCharacter->Implements<UCombatInterface>() || ICombatInterface::Execute_IsDead(Props.TargetCharacter)) { return; }
	
	if (Data.EvaluatedData.Attribute == GetManaAttribute())
	{
		// TODO: Should we do this here? Seems to result in server receiving an extra mana changed event, not client
		//const float ClampedMana = FMath::Clamp(GetMana(), 0.f, GetMaxMana());
		//UE_LOG(LogTemp, Warning, TEXT("Post GE Mana value: %f, setting Mana clamped: %f"), GetMana(), ClampedMana);
		//SetMana(ClampedMana);
	}
	
	// If we are getting a changed incoming damage (meta attribute)
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		// See lecture 314 if this solution isn't sufficient for stopping hit react
		const bool bDotDamage = Data.EffectSpec.GetDuration() > 0.f;
		HandleIncomingDamage(Props, bDotDamage);
	}
	
	// If we are getting a changed incoming xp (meta attribute)
	if (Data.EvaluatedData.Attribute == GetIncomingXpAttribute())
	{
		HandleIncomingXP(Props);
	}

	//if (Data.EvaluatedData.Attribute == GetStrengthAttribute())
	//{
	//}
}

void UAuraAttributeSet::HandleIncomingDamage(const FEffectProperties& Props, const bool bDotDamage)
{
	// Use the meta-attribute value, then consume the data and zero it out
	const float LocalIncomingDamage = GetIncomingDamage();
	SetIncomingDamage(0.f);
	if (LocalIncomingDamage <= 0.f) { return; }

	const float NewHealth = GetHealth() - LocalIncomingDamage;
	
	// Clamp min health to 1 for player's character so we never die
	const float MinHealth = IsValid(Props.TargetController) ? 1.f : 0.f;
	SetHealth(FMath::Clamp(NewHealth, MinHealth, GetMaxHealth()));

	// If fatal damage
	if (GetHealth() <= 0.f)
	{
		// Call die function on combat interface of target actor if fatal damage
		ICombatInterface* CombatInterface = CastChecked<ICombatInterface>(Props.TargetAvatarActor);
		CombatInterface->Die(UAuraAbilitySystemLibrary::GetDeathImpulse(Props.EffectContextHandle));
		SendXpEvent(Props);
	}
	else
	{
		// Knockback character
		const FVector Knockback = UAuraAbilitySystemLibrary::GetKnockback(Props.EffectContextHandle);
		if (!Knockback.IsNearlyZero(1.f) && Props.TargetCharacter)
		{
			Props.TargetCharacter->LaunchCharacter(Knockback, true, true);
			// TODO: Could make interface function for knockback so only AI will stop movement, see lecture 315 comment
			// This will fix the AI Move To task that breaks launch character causing it to only launch directly upwards
			// Could also call Props.TargetCharacter->GetCharacterMovement()->SetMovementMode(MOVE_Falling)
			Props.TargetCharacter->GetCharacterMovement()->StopMovementImmediately();
		}
		
		// Cause hit react
		// See lecture 314 if this solution isn't sufficient for stopping hit react
		if (!bDotDamage && Props.TargetASC)
		{
			// Only hit react if target is not being shocked
			const bool bShouldHitReact = Props.TargetCharacter && Props.TargetCharacter->Implements<UCombatInterface>() && !ICombatInterface::Execute_IsBeingShocked(Props.TargetCharacter);
			if (bShouldHitReact)
			{
				const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
					
				// Create tag container with hit react tag
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(AuraGameplayTags.Abilities_HitReact);
					
				// Cancel any active hit react ability
				Props.TargetASC->CancelAbilities(&TagContainer, nullptr, nullptr);
					
				// Activate hit react gameplay ability
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer);
			}
		}
		
		// Apply debuff if it is successful
		if (!bDotDamage && UAuraAbilitySystemLibrary::IsDebuffSuccessful(Props.EffectContextHandle))
		{
			Debuff(Props);
		}
	}

	// Show floating text
	const bool bBlock = UAuraAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
	const bool bCrit = UAuraAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
	ShowFloatingText(Props, LocalIncomingDamage, bBlock, bCrit);
}

void UAuraAttributeSet::Debuff(const FEffectProperties& Props)
{
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
	FGameplayEffectContextHandle EffectContextHandle = Props.SourceASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(Props.SourceAvatarActor);

	const FGameplayTag DebuffDamageType = UAuraAbilitySystemLibrary::GetDamageType(Props.EffectContextHandle);
	const float DebuffDamage = UAuraAbilitySystemLibrary::GetDebuffDamage(Props.EffectContextHandle);
	const float DebuffFrequency = UAuraAbilitySystemLibrary::GetDebuffFrequency(Props.EffectContextHandle);
	const float DebuffDuration = UAuraAbilitySystemLibrary::GetDebuffDuration(Props.EffectContextHandle);
	
	const FString DebuffName = FString::Printf(TEXT("DynamicDebuff_%s"), *DebuffDamageType.ToString());
	UGameplayEffect* Effect = NewObject<UGameplayEffect>(GetTransientPackage(), FName(DebuffName));
	
	Effect->DurationPolicy = EGameplayEffectDurationType::HasDuration;
	Effect->Period = DebuffFrequency;
	Effect->DurationMagnitude = FScalableFloat(DebuffDuration);
	
	// Add owned gameplay tag of the debuff damage type
	UTargetTagsGameplayEffectComponent& Component = Effect->FindOrAddComponent<UTargetTagsGameplayEffectComponent>();
	FInheritedTagContainer TagContainer = FInheritedTagContainer();
	const FGameplayTag DebuffTag = AuraGameplayTags.DamageTypesToDebuffs[DebuffDamageType];
	TagContainer.Added.AddTag(DebuffTag);
	
	// Stun debuff should also block player input and cursor trace
	const bool bStunDebuff = DebuffTag.MatchesTagExact(AuraGameplayTags.Debuff_Stun);
	if (bStunDebuff)
	{
		TagContainer.Added.AddTag(AuraGameplayTags.Player_Block_InputPressed);
		TagContainer.Added.AddTag(AuraGameplayTags.Player_Block_InputReleased);
		TagContainer.Added.AddTag(AuraGameplayTags.Player_Block_InputHeld);
		TagContainer.Added.AddTag(AuraGameplayTags.Player_Block_CursorTrace);
	}
	Component.SetAndApplyTargetTagChanges(TagContainer);
	
	// TODO: Set stacking type not working, editor only function call, do we need to do this?
	//Effect->SetStackingType(EGameplayEffectStackingType::AggregateBySource);
	
	Effect->StackLimitCount = 1;
	// Do not execute on application so damage text does not overlap on screen
	Effect->bExecutePeriodicEffectOnApplication = false;

	// Use exec calc calculation class to utilize resistances for debuffs
	FGameplayEffectExecutionDefinition Execution;
	Execution.CalculationClass = UExecCalc_Damage::StaticClass();
	Effect->Executions.Add(Execution);
	
	/*
	FGameplayModifierInfo& Modifier = Effect->Modifiers.Add_GetRef(FGameplayModifierInfo());
	Modifier.ModifierMagnitude = FScalableFloat(DebuffDamage);
	Modifier.ModifierOp = EGameplayModOp::Additive;
	Modifier.Attribute = GetIncomingDamageAttribute();
	*/

	// Make a GE spec with an effect context handle and an effect
	if (FGameplayEffectSpec* MutableSpec = new FGameplayEffectSpec(Effect, EffectContextHandle, 1.f))
	{
		// Damage value for UExecCalc_Damage to use
		MutableSpec->SetSetByCallerMagnitude(DebuffDamageType, DebuffDamage);
		
		// Get the effect context casted to our custom type to set the damage type property
		FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(MutableSpec->GetContext().Get());
		AuraContext->SetDamageType(DebuffDamageType);
		
		// Cancelling non-passive abilities upon stun debuff, done in character base stun tag changed instead of here
		
		Props.TargetASC->ApplyGameplayEffectSpecToSelf(*MutableSpec);
	}
}

void UAuraAttributeSet::HandleIncomingXP(const FEffectProperties& Props)
{
	const float LocalIncomingXp = GetIncomingXp();
	SetIncomingXp(0.f);
	UE_LOG(LogAuraAbilitySystem, Warning, TEXT("Post GE Incoming Xp value: %f"), LocalIncomingXp);
		
	if (LocalIncomingXp > 0.f)
	{
		// Source character is the owner, since GA_ListenForEvents applies GE_EventBasedEffect to self, adding to IncomingXp
		if (Props.SourceCharacter && Props.SourceCharacter->Implements<UPlayerInterface>())
		{
			// This will add to the xp on the player state which will handle everything else
			IPlayerInterface::Execute_AddToXp(Props.SourceCharacter, LocalIncomingXp);
		}
	}
}

void UAuraAttributeSet::ShowFloatingText(const FEffectProperties& EffectProperties, float Damage, bool bBlockedHit, bool bCriticalHit)
{
	if (EffectProperties.SourceCharacter != EffectProperties.TargetCharacter)
	{
		// TODO: This isn't working for client and server
		//if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(UGameplayStatics::GetPlayerController(EffectProperties.SourceCharacter, 0)))
		//if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(EffectProperties.SourceController))

		// If source is aura (aura doing damage), get PC from source
		if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(EffectProperties.SourceCharacter ? EffectProperties.SourceCharacter->GetController() : nullptr))
		{
			AuraPC->ShowDamageNumber(Damage, EffectProperties.TargetCharacter, bBlockedHit, bCriticalHit);
			return;
		}
		
		// If target is aura (enemy doing damage), get PC from target
		if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(EffectProperties.TargetCharacter ? EffectProperties.TargetCharacter->GetController() : nullptr))
		{
			AuraPC->ShowDamageNumber(Damage, EffectProperties.TargetCharacter, bBlockedHit, bCriticalHit);
		}
		
		// Solution for now, only local controller should execute
		// TODO: Only damage dealer should be showing the damage number, correct?
		// for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
		// {
		// 	if (AAuraPlayerController* AuraPC = Cast<AAuraPlayerController>(It->Get()))
		// 	{
		// 		AuraPC->ShowDamageNumber(Damage, EffectProperties.TargetCharacter, bBlockedHit, bCriticalHit);
		// 	}
		// }
	}
}

void UAuraAttributeSet::SendXpEvent(const FEffectProperties& Props)
{
	if (Props.TargetAvatarActor && Props.TargetAvatarActor->Implements<UCombatInterface>())
	{
		// Get the target avatar level and class
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetAvatarActor);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetAvatarActor);

		// Get the xp reward amount from target avatar
		const int32 XpReward = UAuraAbilitySystemLibrary::GetXpRewardFromEnemy(Props.TargetAvatarActor, TargetClass, TargetLevel);
	
		const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();
	
		FGameplayEventData Payload;
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXp;
		Payload.EventMagnitude = XpReward;

		// Send gameplay event using given gameplay tag of Attributes_Meta_IncomingXp and magnitude of the xp
		// GA_ListenForEvents will receive this gameplay event and use GE_EventBasedEffect to make OutgoingSpec and set by caller the magnitude to equal
		// this event's magnitude (xp reward). Then it will apply the GE to self (source character), which adds to the meta attribute IncomingXp
		// which will be received in PostGameplayEffectExecute() where this IncomingXp value can be utilized.
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXp, Payload);
	}
}

UAuraAbilitySystemComponent* UAuraAttributeSet::GetAuraAbilitySystemComponent() const
{
	return Cast<UAuraAbilitySystemComponent>(GetOwningAbilitySystemComponent());
}

void UAuraAttributeSet::LevelUp()
{
	SetHealth(GetMaxHealth());
	SetMana(GetMaxMana());
}

bool UAuraAttributeSet::CanApplyAttributePoints(const FGameplayTag& AttributeTag, int32 Points) const
{
	if (Points == 0) { return false; }

	// If we are spending points on an attribute, need enough attribute points
	if (Points > 0)
	{
		// Invalid attribute tag means we gained overall attribute points
		if (!AttributeTag.IsValid())
		{
			// Can always gain overall attribute points
			return true;
		}

		// This means we are adding points to a specific attribute, so AttributePoints must be greater or equal
		// Because ultimately we will subtract from AttributePoints in order to add to the attribute (ex. strength)
		if (AActor* Avatar = GetOwningAbilitySystemComponent()->GetAvatarActor())
		{
			if (Avatar->Implements<UPlayerInterface>())
			{
				const int32 AttributePoints = IPlayerInterface::Execute_GetAttributePoints(Avatar);
				return AttributePoints >= Points;
			}
		}
	}
	else // If we are removing points from an attribute, cannot go below attribute base value
	{
		// Cannot "refund" overall attribute points
		if (!AttributeTag.IsValid())
		{
			return false;
		}
		
		const FGameplayAttribute& Attribute = TagsToAttributes.FindChecked(AttributeTag);
		const float CurrentAttributeValue = Attribute.GetNumericValue(this);
		
		// TODO: Check that new value is not less than base(initial) value for that attribute, not 0
		if (static_cast<int32>(CurrentAttributeValue) + Points < 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("Cannot have negative attribute points!"));
			return false;
		}
		return true;
	}
	
	return false;
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

void UAuraAttributeSet::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, FireResistance, OldFireResistance);
}

void UAuraAttributeSet::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, LightningResistance, OldLightningResistance);
}

void UAuraAttributeSet::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, ArcaneResistance, OldArcaneResistance);
}

void UAuraAttributeSet::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, PhysicalResistance, OldPhysicalResistance);
}

void UAuraAttributeSet::FillEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& OutEffectProperties) const
{
	// Source = causer of the effect, Target = target of the effect (owner of this attribute set)
	OutEffectProperties = FEffectProperties(Data);
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
		TargetCharacter = Cast<ACharacter>(TargetAvatarActor);
		
		TargetController = Data.Target.AbilityActorInfo->PlayerController.IsValid() ? Data.Target.AbilityActorInfo->PlayerController.Get(): nullptr;
		
		//TargetASC = &Data.Target;
		//TargetASC = GetOwningAbilitySystemComponent();
		TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetAvatarActor);
	}
}
