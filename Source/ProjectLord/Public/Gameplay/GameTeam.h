// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameTeam.generated.h"

UENUM(BlueprintType)
enum class EGameTeam : uint8
{
    Monster,
    Player1,
    Player2,
    Neutral,

    MAX UMETA(Hidden)
};

ENUM_RANGE_BY_COUNT(EGameTeam, EGameTeam::MAX);
