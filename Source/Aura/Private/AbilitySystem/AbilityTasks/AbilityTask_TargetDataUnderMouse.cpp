// Copyright Berkeley Bidwell

#include "AbilitySystem/AbilityTasks/AbilityTask_TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

UAbilityTask_TargetDataUnderMouse* UAbilityTask_TargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility, FName TaskInstanceName)
{
	UAbilityTask_TargetDataUnderMouse* MyObj = NewAbilityTask<UAbilityTask_TargetDataUnderMouse>(OwningAbility, TaskInstanceName);

	
	return MyObj;
}

void UAbilityTask_TargetDataUnderMouse::Activate()
{
	// Both client and server are in here
	
	if (!Ability)
	{
		return;
	}
	
	const bool bIsLocallyControlled = Ability->IsLocallyControlled();
	
	const FGameplayAbilityActivationInfo ActivationInfo = Ability->GetCurrentActivationInfo();
	const bool bHasAuthority = Ability->HasAuthority(&ActivationInfo);
	
	//const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();
	//const bool bHasAuthority = Info->IsNetAuthority();
	
	UE_LOG(LogTemp, Display, TEXT("Activate TargetDataUnderMouse: Locally controlled: %s, Authority: %s"),
		bIsLocallyControlled ? TEXT("true") : TEXT("false"), bHasAuthority ? TEXT("true") : TEXT("false"));
	
	// 3 cases
	// 1. Client and locally controlled
	// 2. Server and locally controlled
	// 3. Server and NOT locally controlled
	
	if (bIsLocallyControlled)
	{
		// This case is for client and also for server when autonomous proxy
		SendTargetDataToServer();
	}
	else // This case is only for server only when it is not the local controller
	{
		if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
		{
			const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
			const FPredictionKey ActivationKey = GetActivationPredictionKey();
			
			// Bind to the target data set delegate callback (could have been set already, or not)
			//FAbilityTargetDataSetDelegate& TargetDataSetDelegate = ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationKey);
			//TargetDataSetDelegate.AddUObject(this, &UAbilityTask_TargetDataUnderMouse::OnTargetDataReplicatedCallback);
			
			OnTargetDataReadDelegateHandle = ASC->AbilityTargetDataSetDelegate(SpecHandle, ActivationKey).AddUObject(this, &UAbilityTask_TargetDataUnderMouse::OnTargetDataReplicatedCallback);

			// If Target Data is already set then call the delegate again to trigger the TargetDataSetDelegate callback
			// TODO: What should EAbilityGenericReplicatedEvent::Type be?
			const bool bCalledDelegate = ASC->CallReplicatedEventDelegateIfSet(EAbilityGenericReplicatedEvent::Type::GenericSignalFromClient, SpecHandle, ActivationKey);

			// If the delegate was not called this means we are waiting on the target data, need to call this function
			if (!bCalledDelegate)
			{
				SetWaitingOnRemotePlayerData();
			}
		}
	}
}

void UAbilityTask_TargetDataUnderMouse::SendTargetDataToServer()
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!Ability || !ASC)
	{
		return;
	}
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	if (!PC)
	{
		return;
	}
	
	// Says everything in this scope should be predicted
	/** To be called in the callsite where the predictive code will take place.
	 * This generates a new PredictionKey and acts as a synchonization point between client and server for that key.
	 */
	// From AbilityTask_WaitTargetData.cpp
	FScopedPredictionWindow	ScopedPrediction(ASC, ShouldReplicateDataToServer());
	//FScopedPredictionWindow	ScopedPrediction(ASC);
	
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// TODO: Why not use FGameplayAbilityTargetData_LocationInfo
	// Construct TargetData and populate with HitResult
	FGameplayAbilityTargetData_SingleTargetHit* TargetData = new FGameplayAbilityTargetData_SingleTargetHit(CursorHit);
	
	// Construct a target data handle with the target data
	FGameplayAbilityTargetDataHandle DataHandle(TargetData);
	
	// Don't want to do this on server
	if (IsPredictingClient())
	{
		UE_LOG(LogTemp, Warning, TEXT("Is predicting client, sending target data to server"));
		
		// Send the target data handle to the server
		FGameplayTag ApplicationTag;
		ASC->ServerSetReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey(),
			DataHandle, ApplicationTag, ASC->ScopedPredictionKey);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Is NOT predicting client, NOT sending target data to server"));
	}
	
	// Broadcast target data handle locally, want to do for both local controlled server and client
	// Technically client doesn't need to do this since it currently has no predictive functionality with the target data
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		UE_LOG(LogTemp, Warning, TEXT("SendTargetDataToServer(): Broadcast target data"));
		ValidMouseTargetData.Broadcast(DataHandle);
	}

	// Task is complete now
	EndTask();
}

void UAbilityTask_TargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// replicated data in GAS can be client to server also

	// Tell ASC that target data has been received, don't need to store it anymore in cache
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		// TODO: Should we be removing the delegate handle here?
		ASC->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(OnTargetDataReadDelegateHandle);
		ASC->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	}
	
	// Finally, broadcast delegate now that we have target data in the data handle
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		UE_LOG(LogTemp, Warning, TEXT("OnTargetDataReplicatedCallback(): Broadcast target data"));
		ValidMouseTargetData.Broadcast(DataHandle);
	}

	// TODO: Should we end task after broadcasting?
	// We are done. Kill us so we don't keep getting broadcast messages
	EndTask();
}

void UAbilityTask_TargetDataUnderMouse::OnDestroy(bool AbilityIsEnding)
{
	Super::OnDestroy(AbilityIsEnding);

	// Unregister any callbacks

	/*
	if (UAbilitySystemComponent* ASC = AbilitySystemComponent.Get())
	{
		ASC->AbilityTargetDataSetDelegate(GetAbilitySpecHandle(), GetActivationPredictionKey()).Remove(OnTargetDataReadDelegateHandle);
	}
	*/
}

bool UAbilityTask_TargetDataUnderMouse::ShouldReplicateDataToServer() const
{
	if (!Ability)
	{
		return false;
	}

	// Send TargetData to the server IFF we are the client and this isn't a GameplayTargetActor that can produce data on the server	
	const FGameplayAbilityActorInfo* Info = Ability->GetCurrentActorInfo();

	return !Info->IsNetAuthority();
}
