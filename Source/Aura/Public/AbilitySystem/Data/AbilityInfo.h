// Copyright Berkeley Bidwell

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AbilityInfo.generated.h"

class UObject;
class UTexture2D;
class UMaterialInterface;

USTRUCT(BlueprintType)
struct FAuraAbilityInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AbilityTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag CooldownTag = FGameplayTag();

	// Not edit defaults only because this is set through code at runtime
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	// TODO: Maybe make soft ptr
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> Icon;
	
	// TODO: Maybe make soft ptr
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UMaterialInterface> BackgroundMaterial;
};

/**
 * 
 */
UCLASS()
class AURA_API UAbilityInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	FAuraAbilityInfo FindAbilityInfo(const FGameplayTag& AbilityTag, bool bLogNotFound = false) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilityInformation")
	TArray<FAuraAbilityInfo> AbilityInformation;
	
};
