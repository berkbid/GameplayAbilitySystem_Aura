// Copyright Berkeley Bidwell

#include "Player/AuraPlayerState.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
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
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_Level - New Level: %d"), Level);
}
