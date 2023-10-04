// Copyright Berkeley Bidwell

#include "Character/AuraCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Player/AuraPlayerState.h"

AAuraCharacter::AAuraCharacter()
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
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	const ENetRole LocalRole = GetLocalRole();
	UE_LOG(LogTemp, Warning, TEXT("Character: %s, possessedby local role: %s"), *GetName(), *UEnum::GetValueAsString(LocalRole));
	
	// Init for server. Only server is in here, not clients
	InitAbilityActorInfo();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	/*
	// Client doesn't need to do this, client automatically does this UAbilitySystemComponent::OnRep_OwningActor()
	//const ENetRole LocalRole = GetLocalRole();
	//UE_LOG(LogTemp, Warning, TEXT("Character: %s, OnRep_PlayerState by local role: %s"), *GetName(), *UEnum::GetValueAsString(LocalRole));
	// Init for clients. All clients are in here, not server
	//InitAbilityActorInfo();
	*/
}

void AAuraCharacter::InitAbilityActorInfo()
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	const IAbilitySystemInterface* AbilitySystemInterface = CastChecked<IAbilitySystemInterface>(AuraPlayerState);
	
	AbilitySystemComponent = AbilitySystemInterface->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();
	
	check(AbilitySystemComponent);
	
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);

	// Setting initial attribute values using a gameplay effect
	// Technically only server needs to set this as the values will be replicated down to client
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
	}
}
