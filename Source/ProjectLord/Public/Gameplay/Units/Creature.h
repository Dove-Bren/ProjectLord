// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Unit.h"

#include "Creature.generated.h"

class AResidentialBuilding;
class UCombatAttributeSet;
class UCreatureAttributeSet;

// A Unit that moves
UCLASS(Blueprintable)
class PROJECTLORD_API ACreature : public AUnit
{
    GENERATED_BODY()

public:
    ACreature();

    // Old Depreceted
    UUnitType* GetCreatureType() const { return GetUnitType(); }
    AResidentialBuilding* GetHomeBuilding() const { return HomeBuilding.IsValid() ? HomeBuilding.Get() : nullptr; }

    UFUNCTION(BlueprintPure)
    bool HasBuilding() const { return !!GetHomeBuilding(); }

    UFUNCTION(BlueprintCallable)
    void SetHomeBuilding(AResidentialBuilding* Building) { HomeBuilding = Building; }

    UFUNCTION(BlueprintPure)
    UCreatureAttributeSet* GetCreatureAttributeSet() const { return CreatureAttributeSet; }


protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    TWeakObjectPtr<AResidentialBuilding> HomeBuilding;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UCreatureAttributeSet> CreatureAttributeSet;

    virtual void RegisterAttributes() override;

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
};
