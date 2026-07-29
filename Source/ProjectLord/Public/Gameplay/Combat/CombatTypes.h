// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    System,
    Melee,
    Ranged,
    Magic,
};

UENUM(BlueprintType)
enum class EUnitTeam : uint8
{
    Monster,
    Player1,
    Player2,
};