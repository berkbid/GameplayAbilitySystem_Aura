// Copyright Berkeley Bidwell

#include "Player/AuraPlayerState.h"

#include "AuraLogChannels.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
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
	AddOrRefundSpellPoints(TotalSpellPointGain);

	// Broadcast delegate for MMC_MaxHealth and MMC_MaxMana to update before maximizing health and mana below
	OnLevelUpModifierDependencyChange.Broadcast();
	
	// Tell the attribute set to perform level up logic (maximize health and mana). Done after MMC_MaxHealth and MMC_MaxMana recalculate above
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AuraAttributeSet->LevelUp();
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

bool AAuraPlayerState::AddOrRefundSpellPoints(int32 SpellPointsToAdd)
{
	// Must be server and have non-zero spell points to add
	if (!HasAuthority() || SpellPointsToAdd == 0)
	{
		return false;
	}

	const int32 NewSpellPoints = SpellPoints + SpellPointsToAdd;

	// New spell point value must not be same as old, and must be non-zero to process request
	if (NewSpellPoints == SpellPoints || NewSpellPoints < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("AddSpellPoints: INVALID NEW SPELL POINTS - Current SpellPoints: %d, New SpellPoints: %d"), SpellPoints, NewSpellPoints);
		return false;
	}
	
	const int32 OldSpellPoints = SpellPoints;
	SpellPoints += SpellPointsToAdd;
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
