// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/ResidentialBuilding.h"

#include "SpawningBuilding.generated.h"

// A building that naturally spawns creatures over time -- like a sewer or graveyard.
UCLASS(Blueprintable)
class PROJECTLORD_API ASpawningBuilding : public AResidentialBuilding
{
    GENERATED_BODY()

public:

    // TODO: Spawning schedule or AI?
};
