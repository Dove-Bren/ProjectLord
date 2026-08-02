// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/Building.h"

#include "ResidentialBuilding.generated.h"

class ACreature;

// A building that supports visits, residents, or both
UCLASS(Blueprintable)
class PROJECTLORD_API AResidentialBuilding : public ABuilding
{
    GENERATED_BODY()

public:
    AResidentialBuilding();

    const TArray<ACreature*> GetBuildingResidents() const { return Residents; }
    const TArray<ACreature*> GetBuildingVisitors() const { return Visitors; }

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    bool RemoveResident(const ACreature* Resident);

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void RemoveAllResidents();

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    bool AddResident(ACreature* Resident);

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void RemoveAllVisitors();

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void AddVisitor(ACreature* Visitor);

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    void RemoveVisitor(const ACreature* Visitor);

    UFUNCTION(BlueprintCallable)
    void RecruitNewUnit(UUnitType* RecruitType);



protected:

    // Creatures that consider this building their home
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<ACreature*> Residents;

    // Creatures that are actively inside this building instead of out on the map
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<ACreature*> Visitors;

    virtual void HandleDeath() override;

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

};
