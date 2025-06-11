// Copyright Berkeley Bidwell

#include "Character/AuraEnemy.h"
#include "Components/CapsuleComponent.h"
#include "UI/Component/HealthBarWidgetComponent.h"
#include "UI/WidgetController/EnemyWidgetController.h"
#include "UI/Widget/AuraUserWidget.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AuraGameplayTags.h"
#include "AI/AuraAIController.h"
#include "Aura/Aura.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraEnemy)

AAuraEnemy::AAuraEnemy(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// Create widget component for health bar and attach to capsule
	HealthBar = CreateDefaultSubobject<UHealthBarWidgetComponent>(TEXT("HealthBar"));
	HealthBar->SetupAttachment(GetCapsuleComponent());
	
	// Could use mesh if we want to for cursor over events
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// Synchronize the movement max walk speed with local variable BaseWalkSpeed
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	// Can we set this in constructor of aura ability system component?
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// Apparently create default subobject can be used for regular objects that aren't components
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	// Set stencil values for highlight post process
	GetMesh()->CustomDepthStencilValue = CUSTOM_DEPTH_RED;
	if (Weapon)
	{
		Weapon->CustomDepthStencilValue = CUSTOM_DEPTH_RED;
	}

	Tags.Emplace(ACTOR_TAG_ENEMY);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	LeftHandSocketName = FName("LeftHandSocket");
	RightHandSocketName = FName("RightHandSocket");
	TailSocketName = FName("TailSocket");

	// Have AI enemies auto possessed when placed in world or spawned
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();
	
	// Bing to when ability system component gameplay tag Effects.HitReact is added or removed
	const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
	FOnGameplayEffectTagCountChanged& OnGameplayEffectTagCountChanged =
		AbilitySystemComponent->RegisterGameplayTagEvent(AuraGameplayTags.Effects_HitReact, EGameplayTagEventType::NewOrRemoved);
	
	OnGameplayEffectTagCountChanged.AddUObject(this, &AAuraEnemy::OnHitReactTagCountChanged);
	
	// Set the widget controller on the health bar component's widget
	if (UAuraUserWidget* HealthBarAuraUserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		HealthBarAuraUserWidget->SetWidgetController(GetEnemyWidgetController());
	}
}

void AAuraEnemy::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (!HasAuthority())
	{
		return;
	}
	
	if (AAuraAIController* AIController = GetAuraAIController())
	{
		check(BehaviorTree);
		AIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
		AIController->RunBehaviorTree(BehaviorTree);
		// Set some initial blackboard values
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("HitReacting"), false);
		AIController->GetBlackboardComponent()->SetValueAsBool(FName("bRangedAttacker"), CharacterClass != ECharacterClass::Warrior);
	}
}

AAuraAIController* AAuraEnemy::GetAuraAIController() const
{
	return CastChecked<AAuraAIController>(Controller, ECastCheckedType::NullAllowed);
}

void AAuraEnemy::InitAbilityActorInfo()
{
	check(AbilitySystemComponent);

	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	
	// Only doing this as server (values get replicated to clients), course does not do this check
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}
}

void AAuraEnemy::AddCharacterAbilities() const
{
	// Give startup abilities to the enemies, which are located in CharacterClassInfo
	UAuraAbilitySystemLibrary::GiveCommonStartupAbilities(this, AbilitySystemComponent, CharacterClass);
}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	UAuraAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}

void AAuraEnemy::OnHitReactTagCountChanged(const FGameplayTag GameplayTag, int32 TagCount)
{
	// Both server and client are in here
	
	bHitReacting = TagCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;

	// Server update the blackboard value for bHitReacting
	if (HasAuthority())
	{
		if (AAuraAIController* AIController = GetAuraAIController())
		{
			AIController->GetBlackboardComponent()->SetValueAsBool(FName("bHitReacting"), bHitReacting);
		}
	}
}

void AAuraEnemy::Die()
{
	// Server is in here
	
	// Stop the behavior tree
	if (AAuraAIController* AIController = GetAuraAIController())
	{
		if (UBrainComponent* BrainComponent = AIController->GetBrainComponent())
		{
			BrainComponent->StopLogic(TEXT("Died"));
		}
	}
	
	SetLifeSpan(LifeSpan);
	Super::Die();
}

FVector AAuraEnemy::GetCombatSocketLocation_Implementation(const FGameplayTag& MontageTag) const
{
	const FVector& FoundLocation = Super::GetCombatSocketLocation_Implementation(MontageTag);
	if (!FoundLocation.Equals(FVector()))
	{
		return FoundLocation;
	}
	
	const FAuraGameplayTags& GameplayTags = FAuraGameplayTags::Get();

	// Left-hand socket
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		UE_CLOG(LeftHandSocketName.IsNone(), LogTemp, Error, TEXT("%s missing LeftHandSocketName property"), *GetName());
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	
	// Right-hand socket
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		UE_CLOG(RightHandSocketName.IsNone(), LogTemp, Error, TEXT("%s missing RightHandSocketName property"), *GetName());
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	
	// Tail socket
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		UE_CLOG(TailSocketName.IsNone(), LogTemp, Error, TEXT("%s missing TailSocketName property"), *GetName());
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}

void AAuraEnemy::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* AAuraEnemy::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

void AAuraEnemy::MulticastHandleDeath()
{
	// Detach the HealthBarWidgetComponent from the root component (capsule).
	// NOTE: This prevents the HealthBarWidgetComponent from falling off (only in clients). "Show Collision" command
	// shows that the capsule component doesn't fall off, so the root of the issue is still unclear.
	//if (!HasAuthority() && HealthBar)
	//{
	//	HealthBar->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	//}
	
	Super::MulticastHandleDeath();
}

UEnemyWidgetController* AAuraEnemy::GetEnemyWidgetController()
{
	if (!EnemyWidgetController)
	{
		checkf(EnemyWidgetControllerClass, TEXT("Enemy missing enemy widget controller class reference! Please fill out B_EnemyBase"));
		
		EnemyWidgetController = NewObject<UEnemyWidgetController>(this, EnemyWidgetControllerClass);

		FWidgetControllerParams WidgetParams;
		WidgetParams.AbilitySystemComponent = AbilitySystemComponent;
		WidgetParams.AttributeSet = AttributeSet;
		WidgetParams.PlayerState = GetPlayerState();
		
		EnemyWidgetController->SetWidgetControllerParams(WidgetParams);
	}
	
	return EnemyWidgetController;
}

void AAuraEnemy::NotifyActorBeginCursorOver()
{
	Super::NotifyActorBeginCursorOver();

	// Highlights enemy with post process material in post process volume using stencil index 250
	GetMesh()->SetRenderCustomDepth(true);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(true);
	}
}

void AAuraEnemy::NotifyActorEndCursorOver()
{
	Super::NotifyActorEndCursorOver();
	
	// UnHighlights enemy with post process material in post process volume
	GetMesh()->SetRenderCustomDepth(false);
	if (Weapon)
	{
		Weapon->SetRenderCustomDepth(false);
	}
}
