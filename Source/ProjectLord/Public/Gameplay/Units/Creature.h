// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Unit.h"

#include "Creature.generated.h"

class AResidentialBuilding;
class UCombatAttributeSet;
class UCreatureAttributeSet;

// A Unit that moves
UCLASS(Blueprintable, meta = (PrioritizeCategories = "Unit Combat Selection Creature"))
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
    void SetHomeBuilding(AResidentialBuilding* Building);

    UFUNCTION(BlueprintPure)
    UCreatureAttributeSet* GetCreatureAttributeSet() const { return CreatureAttributeSet; }


protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    TWeakObjectPtr<AResidentialBuilding> HomeBuilding;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<UCreatureAttributeSet> CreatureAttributeSet;

    virtual void RegisterAttributes() override;

    virtual void OnDeath_Implementation() override;

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
};
