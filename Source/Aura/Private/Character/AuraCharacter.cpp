// Copyright Berkeley Bidwell

#include "Character/AuraCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"
#include "Aura/Aura.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraCharacter)

AAuraCharacter::AAuraCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->TargetArmLength = 750.f;
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->SetupAttachment(RootComponent);

	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->bUsePawnControlRotation = false; // Fixed cam
	Camera->SetupAttachment(SpringArm);

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);
	// Top down params
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;
	
	//bReplicates = true;

	Tags.Emplace(ACTOR_TAG_PLAYER);
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	const ENetRole LocalRole = GetLocalRole();
	UE_LOG(LogTemp, Warning, TEXT("Character: %s, possessed by local role: %s"), *GetName(), *UEnum::GetValueAsString(LocalRole));
	
	// Init for server. Only server is in here, not clients
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// Apparently client does need to do this here
	// Client doesn't need to do this, client automatically does this UAbilitySystemComponent::OnRep_OwningActor()
	//const ENetRole LocalRole = GetLocalRole();
	//UE_LOG(LogTemp, Warning, TEXT("Character: %s, OnRep_PlayerState by local role: %s"), *GetName(), *UEnum::GetValueAsString(LocalRole));
	//Init for clients. All clients are in here, not server
	InitAbilityActorInfo();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	const IAbilitySystemInterface* AbilitySystemInterface = CastChecked<IAbilitySystemInterface>(AuraPlayerState);
	
	AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	check(AbilitySystemComponent);

	// Call on both client and server
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);

	// This is where course does HUD->InitHUD(), but we are doing that in player controller instead

	
	// Setting initial attribute values using a gameplay effect
	// Technically only server needs to set this as the values will be replicated down to client
	// The course does not do this authority check here
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}
}

void AAuraCharacter::InitializeDefaultAttributes() const
{
	ApplyGameplayEffectToSelf(DefaultPrimaryAttributesClass, 1.f);
	ApplyGameplayEffectToSelf(DefaultSecondaryAttributesClass, 1.f);
	// Set vital after secondary because we want to set health/mana equal to max health/mana
	ApplyGameplayEffectToSelf(DefaultVitalAttributesClass, 1.f);
}

int32 AAuraCharacter::GetPlayerLevel() const
{
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetPlayerState()))
	{
		return CombatInterface->GetPlayerLevel();
	}
	
	return 1;
}
