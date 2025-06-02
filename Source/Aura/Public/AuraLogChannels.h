// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"

class UObject;

AURA_API DECLARE_LOG_CATEGORY_EXTERN(LogAura, Log, All);
AURA_API DECLARE_LOG_CATEGORY_EXTERN(LogAuraAbilitySystem, Log, All);

AURA_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
