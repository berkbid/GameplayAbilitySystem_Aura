// Copyright Berkeley Bidwell

#include "Character/AuraCharacter.h"

#include "AuraGameplayTags.h"
#include "AuraLogChannels.h"
#include "NiagaraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "Player/AuraPlayerState.h"
#include "Aura/Aura.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AuraCharacter)

AAuraCharacter::AAuraCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>("LevelUpNiagaraComponent");
	LevelUpNiagaraComponent->bAutoActivate = false;
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent());
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetUsingAbsoluteRotation(true);
	SpringArm->TargetArmLength = 750.f;
	SpringArm->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));
	SpringArm->bUsePawnControlRotation = false;
	SpringArm->bInheritPitch = false;
	SpringArm->bInheritYaw = false;
	SpringArm->bInheritRoll = false;
	SpringArm->bEnableCameraLag = true;
	SpringArm->SetupAttachment(GetRootComponent());

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

	// Bind to stun tag change
	AbilitySystemComponent->RegisterGameplayTagEvent(FAuraGameplayTags::Get().Debuff_Stun, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &AAuraCharacter::StunTagChanged);
	
	// Call on both client and server
	AbilitySystemComponent->InitAbilityActorInfo(AuraPlayerState, this);

	// Broadcast that the ASC is now registered and available to bind to
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	
	// This is where course does HUD->InitHUD(), but we are doing that in player controller instead

	
	// Setting initial attribute values using a gameplay effect
	// Technically only server needs to set this as the values will be replicated down to client
	// The course does not do this authority check here
	if (HasAuthority())
	{
		InitializeDefaultAttributes();
		AddCharacterAbilities();
	}

	// Bind on both server and clients, both broadcast this event when Level is replicated
	AuraPlayerState->OnLevelUpdate.AddLambda([this](const int32 NewLevel)
	{
		LevelUpParticles();
	});
}

void AAuraCharacter::LevelUpParticles() const
{
	UE_LOG(LogTemp, Warning, TEXT("[%s]: Level up particles"), *GetClientServerContextString(GetController()));
	
	if (IsValid(LevelUpNiagaraComponent))
	{
		// Rotate niagara component towards camera
		const FVector CameraLocation = Camera->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator ToCameraRotation = (NiagaraSystemLocation - CameraLocation).Rotation();
		LevelUpNiagaraComponent->SetWorldRotation(ToCameraRotation);
		
		LevelUpNiagaraComponent->ActivateSystem();
	}
}

void AAuraCharacter::InitializeDefaultAttributes() const
{
	ApplyGameplayEffectToSelf(DefaultPrimaryAttributesClass, 1.f);
	ApplyGameplayEffectToSelf(DefaultSecondaryAttributesClass, 1.f);
	// Set vital after secondary because we want to set health/mana equal to max health/mana
	ApplyGameplayEffectToSelf(DefaultVitalAttributesClass, 1.f);
}

void AAuraCharacter::OnRep_Stunned()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		const FAuraGameplayTags& AuraGameplayTags = FAuraGameplayTags::Get();
		
		// Server is doing this in attribute set where a GE is controlling the stun debuff tag duration
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(AuraGameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(AuraGameplayTags.Player_Block_InputHeld);
		BlockedTags.AddTag(AuraGameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(AuraGameplayTags.Player_Block_InputReleased);
		
		if (bIsStunned)
		{
			ASC->AddLooseGameplayTags(BlockedTags);
			// Seems have to do this for player for client but not for enemy or for server
			StunDebuffComponent->Activate();
		}
		else
		{
			ASC->RemoveLooseGameplayTags(BlockedTags);
			// Seems have to do this for player for client but not for enemy or for server
			StunDebuffComponent->Deactivate();
		}
	}
}

void AAuraCharacter::OnRep_Burning()
{
	// Need to do this here for aura but for enemies the debuffniagaracomponent handles this, also server handles this in the component
	if (bIsBurning)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
}

int32 AAuraCharacter::GetPlayerLevel_Implementation() const
{
	const APlayerState* PS = GetPlayerState();
	if (PS && PS->Implements<UCombatInterface>())
	{
		return ICombatInterface::Execute_GetPlayerLevel(PS);
	}

	return 1;
}

void AAuraCharacter::AddToXp_Implementation(int32 InXp)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	PS->AddXp(InXp);
}

bool AAuraCharacter::AddOrRefundAttributePoints_Implementation(const FGameplayTag& AttributeTag, int32 InAmount)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	return PS->AddOrRefundAttributePoints(AttributeTag, InAmount);
}

bool AAuraCharacter::SpendOrRefundSpellPoints_Implementation(const FGameplayTag& AbilityTag, int32 InAmount)
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	return PS->SpendOrRefundSpellPoints(AbilityTag, InAmount);
}

int32 AAuraCharacter::GetAttributePoints_Implementation() const
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	return PS->GetAttributePoints();
}

int32 AAuraCharacter::GetSpellPoints_Implementation() const
{
	AAuraPlayerState* PS = GetPlayerState<AAuraPlayerState>();
	check(PS);
	return PS->GetSpellPoints();
}
