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
	// Right now I think only the owner needs xp value, not other clients
	DOREPLIFETIME_CONDITION(AAuraPlayerState, Xp, COND_OwnerOnly);
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

void AAuraPlayerState::OnRep_Xp(int32 OldXp)
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Xp - New Xp: %d"), Xp);
	OnXpUpdate.Broadcast(Xp);
}

void AAuraPlayerState::SetLevel(int32 InLevel)
{
	if (!HasAuthority() || InLevel <= Level)
	{
		return;
	}
	
	check(LevelUpInfo);
	
	const int32 OldLevel = Level;
	const int32 LevelDiff = InLevel - Level;
	
	// Handling level up for each level increase one at a time instead of as a batch level up
	for (int32 i = 0; i < LevelDiff; i++)
	{
		const int32 IntermediateLevel = Level + i;
		
		const int32 AttributeGain = LevelUpInfo->GetAttributeGainForLevel(IntermediateLevel);
		const int32 SpellPointGain = LevelUpInfo->GetSpellPointGainForLevel(IntermediateLevel);

		// TODO: 
		// Add to attribute points
		// Add to spell points
		
		
	}

	
	Level = InLevel;
	OnRep_Level(OldLevel);
	
	UAuraAttributeSet* AuraAttributeSet = CastChecked<UAuraAttributeSet>(AttributeSet);
	AuraAttributeSet->SetHealth(AuraAttributeSet->GetMaxHealth());
	AuraAttributeSet->SetMana(AuraAttributeSet->GetMaxMana());
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("[%s]: OnRep_Level - New Level: %d"), *GetClientServerContextString(this), Level);
	OnLevelUpdate.Broadcast(Level);
}
