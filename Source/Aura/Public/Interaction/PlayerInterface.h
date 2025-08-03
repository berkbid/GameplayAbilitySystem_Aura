// Copyright Berkeley Bidwell

#pragma once

#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

class UObject;
struct FGameplayTag;

UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface for aura character to implement and call functions on aura player state such that whoever
 * is calling this interface does not need to be dependent on the aura player state.
 */
class IPlayerInterface
{
	GENERATED_BODY()

public:
	/** Only server can call, must be positive value */
	UFUNCTION(BlueprintNativeEvent)
	void AddToXp(int32 InXp);

	/** Only server can call, returns true if succeeded to increment by the given attribute amount */
	UFUNCTION(BlueprintNativeEvent)
	bool AddOrRefundAttributePoints(const FGameplayTag& AttributeTag, int32 InAmount);

	/** Only server can call, Returns true if succeeded to increment by the given spell point amount */
	UFUNCTION(BlueprintNativeEvent)
	bool AddOrRefundSpellPoints(int32 InAmount);
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetAttributePoints() const;
	
	UFUNCTION(BlueprintNativeEvent)
	int32 GetSpellPoints() const;
};
