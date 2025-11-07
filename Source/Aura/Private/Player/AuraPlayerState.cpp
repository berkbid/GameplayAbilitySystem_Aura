// Copyright Berkeley Bidwell

#include "Player/AuraPlayerState.h"
#include "AuraGameplayTags.h"
#include "Player/AuraPlayerController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "AuraLogChannels.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraPlayerState)

AAuraPlayerState::AAuraPlayerState(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	// Can we set this in constructor of aura ability system component?
	AbilitySystemComponent->SetIsReplicated(true);
	// For mixed replication mode, the OwnerActor's owner MUST be the controller (can call SetOwner() to do this)
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// Can have multiple attribute sets, but must be of different type
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// Want to update fast because ability system component is on player state
	SetNetUpdateFrequency(100.f);
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState, Level);
	
	//DOREPLIFETIME_CONDITION_NOTIFY(AAuraPlayerState, Xp, COND_OwnerOnly, REPNOTIFY_Always);
	// Right now I think only the owner needs xp/attribute/spellpoints value, not other clients
	DOREPLIFETIME_CONDITION(AAuraPlayerState, Xp, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AAuraPlayerState, AttributePoints, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(AAuraPlayerState, SpellPoints, COND_OwnerOnly);
}

AAuraPlayerController* AAuraPlayerState::GetAuraPlayerController() const
{
	return Cast<AAuraPlayerController>(GetOwner());
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return GetAuraAbilitySystemComponent();
}

void AAuraPlayerState::AddXp(int32 XpToAdd)
{
	if (!HasAuthority() || XpToAdd <= 0)
	{
		return;
	}
	
	const int32 OldXp = Xp;
	
	// Client will broadcast xp when receive OnRep
	Xp += XpToAdd;
	
	check(LevelUpInfo);
	const int32 NewLevel = LevelUpInfo->FindLevelForXp(Xp);

	SetLevel(NewLevel);

	// Broadcast new xp amount
	OnRep_Xp(OldXp);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	if (!HasAuthority() || InLevel <= Level)
	{
		return;
	}
	
	check(LevelUpInfo);

	int32 TotalAttributeGain = 0;
	int32 TotalSpellPointGain = 0;
	
	// Accumulate level up rewards depending on number of levels we are increasing
	for (int32 i = Level; i < InLevel; i++)
	{
		TotalAttributeGain += LevelUpInfo->GetAttributeGainForLevel(i);
		TotalSpellPointGain += LevelUpInfo->GetSpellPointGainForLevel(i);
	}

	// Update level
	const int32 OldLevel = Level;
	Level = InLevel;
	// Could do this at end of function
	OnRep_Level(OldLevel);

	// Update attributes and spell points
	AddOrRefundAttributePoints(FGameplayTag(), TotalAttributeGain);
	SpendOrRefundSpellPoints(FGameplayTag(), TotalSpellPointGain);

	// Broadcast delegate for MMC_MaxHealth and MMC_MaxMana to update before maximizing health and mana below
	OnLevelUpModifierDependencyChange.Broadcast();
	
	// Tell the attribute set to perform level up logic (maximize health and mana). Done after MMC_MaxHealth and MMC_MaxMana recalculate above
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AuraAttributeSet->LevelUp();

	// Tell ASC to update the eligibility of abilities now that we reached a new level
	AbilitySystemComponent->UpdateAbilitiesEligibilityFromLevelUp(Level);
}

bool AAuraPlayerState::AddOrRefundAttributePoints(const FGameplayTag& AttributeTag, int32 AttributePointsToAdd)
{
	if (!HasAuthority())
	{
		return false;
	}
	
	// Make sure it is valid to use these attribute points on the given attribute (spend or refund)
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	if (!AuraAttributeSet->CanApplyAttributePoints(AttributeTag, AttributePointsToAdd))
	{
		return false;
	}

	// If attribute tag is valid, we are adding attribute points to an attribute (ex. strength), and removing from attributepoints
	const int32 AttributePointDelta = AttributeTag.IsValid() ? -AttributePointsToAdd : AttributePointsToAdd;
	
	const int32 OldAttributePoints = AttributePoints;
	AttributePoints = AttributePoints + AttributePointDelta;
	OnRep_AttributePoints(OldAttributePoints);

	return true;
}

bool AAuraPlayerState::SpendOrRefundSpellPoints(const FGameplayTag& AbilityTag, int32 InSpellPoints)
{
	// TODO: Could check if ability tag matches the parent tag of abilities for it to be technically valid
	
	// Must be server and have non-zero spell points to add, also if not valid ability tag, points must be positive
	if (!HasAuthority() || InSpellPoints == 0 || (!AbilityTag.IsValid() && InSpellPoints < 0))
	{
		return false;
	}

	// If we are refunding a specific ability's spell points
	if (AbilityTag.IsValid() && InSpellPoints < 0)
	{
		if (FGameplayAbilitySpec* AbilitySpec = AbilitySystemComponent->GetSpecFromAbilityTag(AbilityTag))
		{
			const FGameplayTag& AbilityStatus = AbilitySystemComponent->GetStatusTagFromSpec(*AbilitySpec);
			
			if (AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Eligible) || AbilityStatus.MatchesTagExact(FAuraGameplayTags::Get().Abilities_Status_Locked))
			{
				// Cannot refund spell points for abilities that are not even unlocked
				return false;
			}
			else // Unlocked or equipped
			{
				// Ability needs enough levels to refund InSpellPoints, can go to level 0
				const int32 AbilityLevel = AbilitySpec->Level;
				if (AbilityLevel + InSpellPoints < 0)
				{
					return false;
				}
			}
		}
	}
	
	// If valid ability tag, we are spending the incoming points, else we are adding more points
	const int32 NewSpellPoints = AbilityTag.IsValid() ? SpellPoints - InSpellPoints : SpellPoints + InSpellPoints;
	
	if (NewSpellPoints == SpellPoints || NewSpellPoints < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AddSpellPoints: INVALID NEW SPELL POINTS - Current SpellPoints: %d, New SpellPoints: %d"), SpellPoints, NewSpellPoints);
		return false;
	}
	
	const int32 OldSpellPoints = SpellPoints;
	SpellPoints = NewSpellPoints;
	OnRep_SpellPoints(OldSpellPoints);

	return true;
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s]: OnRep_Level - New Level: %d"), *GetClientServerContextString(this), Level);
	OnLevelUpdate.Broadcast(Level);
}

void AAuraPlayerState::OnRep_Xp(int32 OldXp)
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Xp - New Xp: %d"), Xp);
	OnXpUpdate.Broadcast(Xp);
}

void AAuraPlayerState::OnRep_AttributePoints(int32 OldAttributePoints)
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_AttributePoints - New AttributePoints: %d"), AttributePoints);
	OnAttributeUpdate.Broadcast(AttributePoints);
}

void AAuraPlayerState::OnRep_SpellPoints(int32 OldSpellPoints)
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_SpellPoints - New SpellPoints: %d"), SpellPoints);
	OnSpellPointUpdate.Broadcast(SpellPoints);
}
