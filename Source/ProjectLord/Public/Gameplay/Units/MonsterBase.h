// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Unit.h"

#include "MonsterBase.generated.h"

UCLASS(Blueprintable)
class PROJECTLORD_API AMonsterBase : public AUnit
{
    GENERATED_BODY()

public:
    AMonsterBase();

    UFUNCTION(BlueprintPure)
    int GetGold() const { return Gold; }

protected:

    // How much treasure this monster is carrying
    UPROPERTY(EditDefaultsOnly, Category = "Resources", Meta=(ClampMin=0))
    int Gold;

};
