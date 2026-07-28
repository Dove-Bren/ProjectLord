// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Unit.h"

#include "Creature.generated.h"

class ABuilding;

UCLASS(Blueprintable)
class PROJECTLORD_API ACreature : public AUnit
{
    GENERATED_BODY()

public:
    ACreature();

    ABuilding* GetHomeBuilding() const { return HomeBuilding.IsValid() ? HomeBuilding.Get() : nullptr; }

    UFUNCTION(BlueprintPure)
    bool HasBuilding() const { return !!GetHomeBuilding(); }

    UFUNCTION(BlueprintCallable)
    void SetHomeBuilding(ABuilding* Building) { HomeBuilding = Building; }


protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    TWeakObjectPtr<ABuilding> HomeBuilding;
};
