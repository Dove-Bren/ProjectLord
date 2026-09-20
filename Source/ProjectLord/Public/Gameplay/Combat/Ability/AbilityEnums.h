// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AbilityEnums.generated.h"

UENUM(BlueprintType)
enum class EAbilityTargetType : uint8
{
	Enemy,
	Ally,
	Self,
	Heal,
};

UENUM(BlueprintType)
enum class EAbilityAnimType : uint8
{
	HarmingNormal,
	HarmingLarge,
	HelpingNormal,
	HelpingLarge,
};

UCLASS()
class PROJECTLORD_API UAbilityEnumsFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintPure, Category = "Ability")
	static FGameplayTag GetTagForTargetType(EAbilityTargetType Type);

};

