// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Unit.h"

#include "Creature.generated.h"

class ABuilding;
class UCreatureType;
class UCombatAttributeSet;
class UCreatureAttributeSet;

UCLASS(Blueprintable)
class PROJECTLORD_API ACreature : public AUnit
{
    GENERATED_BODY()

public:
    ACreature();

    UCreatureType* GetCreatureType() const { return CreatureType; }
    ABuilding* GetHomeBuilding() const { return HomeBuilding.IsValid() ? HomeBuilding.Get() : nullptr; }

    UFUNCTION(BlueprintPure)
    bool HasBuilding() const { return !!GetHomeBuilding(); }

    UFUNCTION(BlueprintCallable)
    void SetHomeBuilding(ABuilding* Building) { HomeBuilding = Building; }

    UFUNCTION(BlueprintPure)
    UCombatAttributeSet* GetCombatAttributeSet() const { return CombatAttributeSet; }

    UFUNCTION(BlueprintPure)
    UCreatureAttributeSet* GetCreatureAttributeSet() const { return CreatureAttributeSet; }


protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Creature")
    TObjectPtr<UCreatureType> CreatureType;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Creature")
    TWeakObjectPtr<ABuilding> HomeBuilding;

    // MOVE HERE
    /*UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UCombatAttributeSet> CombatAttributeSet;*/

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<UCreatureAttributeSet> CreatureAttributeSet;

    /*MOVE HERE UPROPERTY(EditDefaultsOnly, Category = "Attributes", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.UnitBaseAttributes"))
    TObjectPtr<UDataTable> ClassAttributeDefaults;*/

    UFUNCTION()
    void HandleDeath();

    virtual void RegisterAttributes();
    virtual void SetupBaseAttributes();

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;
};
