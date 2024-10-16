// Copyright Berkeley Bidwell

#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
//#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"

UAbilityTask_TargetDataUnderMouse* UAbilityTask_TargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAbilityTask_TargetDataUnderMouse* MyObj = NewAbilityTask<UAbilityTask_TargetDataUnderMouse>(OwningAbility, TaskInstanceName);

	
	return MyObj;
}

void UAbilityTask_TargetDataUnderMouse::Activate()
{
	if (Ability == nullptr)
	{
		return;
	}
	
	// Both client and server are in here
	// Want to get line trace hit result under cursor and broadcast that location
	
	if (APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get())
	{
		FHitResult Hit;
		PC->GetHitResultUnderCursor(ECC_Visibility, false, Hit);
		UE_LOG(LogTemp, Warning, TEXT("Cursor hit location: %s"), *Hit.Location.ToString());
		
		ValidMouseTargetData.Broadcast(Hit.Location);
	}

	/*
	//GetOwnerActor();
	//GetAvatarActor();
	//if (APlayerState* AbilityPlayerState = Cast<APlayerState>(Ability->GetGameplayTaskOwner(this)))
	if (APlayerState* AbilityPlayerState = Cast<APlayerState>(GetOwnerActor()))
	{
		if (APlayerController* AbilityPlayerController = Cast<APlayerController>(AbilityPlayerState->GetOwner()))
		{
			//FVector ReturnTargetData;
			//AbilityPlayerController->GetMousePosition(ReturnTargetData.X, ReturnTargetData.Y);
			
			FVector WorldLocation;
			FVector WorldDirection;
			AbilityPlayerController->DeprojectMousePositionToWorld(WorldLocation, WorldDirection);
			UE_LOG(LogTemp, Warning, TEXT("Found world location: %s"), *WorldLocation.ToString());
			
			ValidMouseTargetData.Broadcast(WorldLocation);
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("did not find player controller owner for player state"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("did not find gameplay task owner as player state"));
	}
	*/
	
	// if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	// {
	// 	ASC->GetOwnerActor();
	// }
}

void UAbilityTask_TargetDataUnderMouse::OnDestroy(bool AbilityIsEnding)
{
	Super::OnDestroy(AbilityIsEnding);

	// Unregister any callbacks
	
}
