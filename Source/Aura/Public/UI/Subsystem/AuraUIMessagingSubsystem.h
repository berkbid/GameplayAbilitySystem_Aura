// Copyright Berkeley Bidwell

#pragma once

#include "Messaging/CommonMessagingSubsystem.h"
#include "AuraUIMessagingSubsystem.generated.h"

class FSubsystemCollectionBase;
class UCommonGameDialog;
class UCommonGameDialogDescriptor;
class UObject;

/**
 * 
 */
UCLASS()
class AURA_API UAuraUIMessagingSubsystem : public UCommonMessagingSubsystem
{
	GENERATED_BODY()
	
public:
	UAuraUIMessagingSubsystem() { }

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	virtual void ShowConfirmation(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;
	virtual void ShowError(UCommonGameDialogDescriptor* DialogDescriptor, FCommonMessagingResultDelegate ResultCallback = FCommonMessagingResultDelegate()) override;
	
};
