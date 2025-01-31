// Copyright Berkeley Bidwell

#pragma once

#include "CommonPlayerController.h"
#include "AuraFrontEndPlayerController.generated.h"

class UObject;

/**
 * 
 */
UCLASS()
class AURA_API AAuraFrontEndPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()
	
public:
	AAuraFrontEndPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
