// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Buildings/Building.h"

#include "ResidentialBuilding.generated.h"

class ACreature;
class UUnitType;

DECLARE_MULTICAST_DELEGATE(FOnResidentsChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResidentAdded, ACreature*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResidentRemoved, ACreature*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnResidentLimitChanged, UUnitType*);

DECLARE_MULTICAST_DELEGATE(FOnVisitorsChanged);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnVisitorAdded, ACreature*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnVisitorRemoved, ACreature*);

// A building that supports visits, residents, or both
UCLASS(Blueprintable)
class PROJECTLORD_API AResidentialBuilding : public ABuilding
{
    GENERATED_BODY()

public:
    AResidentialBuilding();

    FOnResidentsChanged OnResidentsChanged;
    FOnResidentAdded OnResidentAdded;
    FOnResidentRemoved OnResidentRemoved;
    FOnResidentLimitChanged OnResidentLimitChanged;

    FOnVisitorsChanged OnVisitorsChanged;
    FOnVisitorAdded OnVisitorAdded;
    FOnVisitorRemoved OnVisitorRemoved;

    const TArray<ACreature*> GetBuildingResidents() const { return Residents; }
    const TArray<ACreature*> GetBuildingVisitors() const { return Visitors; }

    // Note: Does not clean up references on the Creature
    UFUNCTION(BlueprintCallable)
    bool RemoveResident(ACreature* Resident);

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
    void RemoveVisitor(ACreature* Visitor);

    UFUNCTION(BlueprintCallable)
    void RecruitNewUnit(UUnitType* RecruitType);

    UFUNCTION(BlueprintPure)
    int GetResidentTypeCount(const UUnitType* Type, bool bIncludeQueue = true) const;

    UFUNCTION(BlueprintPure)
    int GetResidentTypeLimit(const UUnitType* Type) const;

    UFUNCTION(BlueprintPure)
    bool CanFitResidentType(const UUnitType* Type) const;

    UFUNCTION(BlueprintCallable)
    void SetResidentTypeLimit(UUnitType* Type, int Limit);

    UFUNCTION(BlueprintPure)
    TSet<UUnitType*> GetAllResidentTypes() const;

    UFUNCTION(BlueprintPure)
    int GetVisitorCount() const { return Visitors.Num(); }

    virtual void SetupSelectionData(USelectionComponent* InSelectionComponent) override;

protected:

    // Creatures that consider this building their home
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<ACreature*> Residents;

    // Creatures that are actively inside this building instead of out on the map
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Building|Contents")
    TArray<ACreature*> Visitors;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building|Residents")
    TMap<UUnitType*, int> ResidentTypeLimits;

    virtual void HandleDeath() override;
    virtual int GetResidentsInQueue(const UUnitType* Type) const { return 0; }

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

};
