// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameTeam.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/Castle.h"
#include "Gameplay/Buildings/BuildingTypes.h"
#include "Gameplay/Units/Unit.h"
#include "Gameplay/Units/UnitTypes.h"
#include "UI/ViewModels/GameTeamStateViewModel.h"

AGameTeamState::AGameTeamState()
{
	Gold = 0;
}

void AGameTeamState::BeginPlay()
{
    Super::BeginPlay();

    ViewModel = CreateLordVM<UVMGameTeamState>(this);
    ViewModel->Setup(this);
}

void AGameTeamState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ThisClass, Team);
    DOREPLIFETIME(ThisClass, Gold);
    DOREPLIFETIME(ThisClass, TeamCastle);
    DOREPLIFETIME(ThisClass, TeamUnits);
    DOREPLIFETIME(ThisClass, TeamFlags);
}

void AGameTeamState::SetTeam(EGameTeam InTeam)
{
    Team = InTeam;
    ViewModel->SetTeam(InTeam);
}

int AGameTeamState::AddGold(int InChange)
{
    Gold = FMath::Clamp(Gold + InChange, 0, 9999999);
    OnTeamGoldChanged.Broadcast(Gold);
    return Gold;
}

void AGameTeamState::SetCastle(ACastle* Castle)
{
    if (!TeamCastle.IsExplicitlyNull())
    {
        RemoveBuilding(TeamCastle.Get());
    }
    TeamCastle = Castle;
    if (Castle)
    {
        AddBuilding(Castle);
    }
}

void AGameTeamState::AddUnit(AUnit* Unit)
{
    if (!TeamUnits.Contains(Unit))
    {
        TeamUnits.Add(Unit);
        Unit->OnUnitFinalDeath.AddDynamic(this, &AGameTeamState::OnUnitFinalDeath);
        OnTeamUnitsChanged.Broadcast();
    }
}

void AGameTeamState::RemoveUnit(AUnit* Unit)
{
    if (TeamUnits.Remove(Unit))
    {
        OnTeamUnitsChanged.Broadcast();
        Unit->OnUnitFinalDeath.RemoveAll(this);

        // Clean up worker maps
        NotifyRepairWorkerAbandoned(Unit, nullptr);
    }
}

void AGameTeamState::AddBuilding(ABuilding* Building)
{
    if (!TeamBuildings.Contains(Building))
    {
        TeamBuildings.Add(Building);
        Building->OnBuildingDestroyed.AddUObject(this, &ThisClass::RemoveBuilding);
        OnTeamBuildingsChanged.Broadcast();
    }
}

void AGameTeamState::RemoveBuilding(ABuilding* Building)
{
    if (TeamBuildings.Remove(Building))
    {
        OnTeamBuildingsChanged.Broadcast();
        Building->OnBuildingDestroyed.RemoveAll(this);

        // Clean up worker maps
        RepairWorkers.Remove(Building);
    }
}

void AGameTeamState::AddFlag(ARewardFlag* Flag)
{
    TeamFlags.Add(Flag);
    OnTeamFlagsChanged.Broadcast();
}

void AGameTeamState::RemoveFlag(ARewardFlag* Flag)
{
    if (TeamFlags.Remove(Flag))
    {
        OnTeamFlagsChanged.Broadcast();
    }
}

TArray<AUnit*> AGameTeamState::GetTeamUnitsOfClass(TSubclassOf<AUnit> Type) const
{
    TArray<AUnit*> Units;
    for (auto Unit : TeamUnits)
    {
        if (Unit->IsA(Type))
        {
            Units.Add(Unit);
        }
    }
    return Units;
}

TArray<AUnit*> AGameTeamState::GetTeamUnitsOfType(const UUnitType* Type) const
{
    return GetTeamUnitsOfClass(Type->UnitClass);
}

TArray<ABuilding*> AGameTeamState::GetTeamBuildingsOfClass(TSubclassOf<ABuilding> Type) const
{
    TArray<ABuilding*> Buildings;
    for (auto Building : TeamBuildings)
    {
        if (Building->IsA(Type))
        {
            Buildings.Add(Building);
        }
    }
    return Buildings;
}

TArray<ABuilding*> AGameTeamState::GetTeamBuildingsOfType(const UBuildingType* Type) const
{
    return GetTeamBuildingsOfClass(Type->BuildingClass);
}

ABuilding* AGameTeamState::GetNextBuildingToRepair(AUnit* Worker)
{
    // Iterate buildings, looking for ones that need repaired.
    // Note: iterate from closest to unit to furthest
    if (!ensure(!TeamBuildings.IsEmpty())) // At least castle should be there
    {
        return nullptr;
    }

    auto UnitPos = Worker->GetActorLocation();
    TeamBuildings.Sort([UnitPos](const ABuilding& Left, const ABuilding& Right)
        {
            // return if Left should come before Right
            return FVector::DistSquaredXY(UnitPos, Left.GetActorLocation())
                < FVector::DistSquaredXY(UnitPos, Right.GetActorLocation());
        });

    for (auto Building : TeamBuildings)
    {
        if (Building->WantsRepair())
        {
            auto& Workers = RepairWorkers.FindOrAdd(Building);
            constexpr int RepairWorkersPerBuilding = 2;
            if (Workers.Num() < RepairWorkersPerBuilding)
            {
                Workers.Add(Worker);
                return Building;
            }
        }
    }

    return nullptr;
}

void AGameTeamState::NotifyBuildingRepairComplete(ABuilding* Building)
{
    RepairWorkers.Remove(Building);
}

void AGameTeamState::NotifyRepairWorkerAbandoned(AUnit* Worker, ABuilding* BuildingOptional)
{
    if (BuildingOptional)
    {
        auto& Workers = RepairWorkers.FindOrAdd(BuildingOptional);
        Workers.Remove(Worker);
    }
    else
    {
        for (auto& [Building, Workers] : RepairWorkers)
        {
            Workers.Remove(Worker);
        }
    }
}

void AGameTeamState::OnUnitFinalDeath(AUnit* Unit)
{
    RemoveUnit(Unit);
}

