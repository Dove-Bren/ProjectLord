// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/ResidentialBuilding.h"

#include "Gameplay/Units/Creature.h"
#include "Gameplay/Units/UnitTypes.h"

AResidentialBuilding::AResidentialBuilding()
{
}

void AResidentialBuilding::BeginPlay()
{
	Super::BeginPlay();
}

void AResidentialBuilding::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);
}

bool AResidentialBuilding::RemoveResident(ACreature* Resident)
{
    if (Residents.RemoveAll([Resident](ACreature* const InResident) { return Resident == InResident; }) > 0)
    {
        OnResidentsChanged.Broadcast();
        OnResidentRemoved.Broadcast(Resident);
        return true;
    }

    return false;
}

void AResidentialBuilding::RemoveAllResidents()
{
    auto Copy = Residents;
    Residents.Empty();

    OnResidentsChanged.Broadcast();
    for (auto Resident : Copy)
    {
        OnResidentRemoved.Broadcast(Resident);
    }
}

bool AResidentialBuilding::AddResident(ACreature* Resident)
{
    if (CanFitResidentType(Resident->GetUnitType()))
    {
        Residents.Add(Resident);
        OnResidentsChanged.Broadcast();
        OnResidentAdded.Broadcast(Resident);
        return true;
    }

    return false;
}

void AResidentialBuilding::RemoveAllVisitors()
{
    auto Copy = Visitors;
	Visitors.Empty();

    OnVisitorsChanged.Broadcast();
    for (auto Visitor : Copy)
    {
        OnVisitorRemoved.Broadcast(Visitor);
    }
}

void AResidentialBuilding::AddVisitor(ACreature* Visitor)
{
	Visitors.Add(Visitor);
    OnVisitorsChanged.Broadcast();
    OnVisitorAdded.Broadcast(Visitor);
}

void AResidentialBuilding::RemoveVisitor(ACreature* Visitor)
{
    if (Visitors.RemoveAll([Visitor](ACreature* const InVisitor) { return Visitor == InVisitor; }))
    {
        OnVisitorsChanged.Broadcast();
        OnVisitorRemoved.Broadcast(Visitor);
    }
}

void AResidentialBuilding::RecruitNewUnit(UUnitType* RecruitType)
{
    auto World = GetWorld();
    if (!ensure(World))
    {
        return;
    }

    auto Location = GetBuildingEntrance();
    auto Rotation = FRotator();
    ACreature* Recruit = World->SpawnActor<ACreature>(RecruitType->UnitClass, Location, Rotation);
    if (IsValid(Recruit))
    {
        Recruit->SetTeam(GetTeam());
        AddResident(Recruit);
        Recruit->SetHomeBuilding(this);
    }
}

void AResidentialBuilding::HandleDeath()
{
    // TODO: Eject and vacate!
    for (auto& Visitor : Visitors)
    {

    }

    for (auto& Resident : Residents)
    {

    }

    Super::HandleDeath();
}

int AResidentialBuilding::GetResidentTypeCount(const UUnitType* Type, bool bIncludeQueue) const
{
    int Count = 0;
    for (const auto Resident : Residents)
    {
        if (Resident->GetUnitType() == Type)
        {
            Count++;
        }
    }
    
    if (bIncludeQueue)
    {
        Count += GetResidentsInQueue(Type);
    }

    return Count;
}

int AResidentialBuilding::GetResidentTypeLimit(const UUnitType* Type) const
{
    if (ResidentTypeLimits.Contains(Type))
    {
        return ResidentTypeLimits[Type];
    }

    return 0;
}

bool AResidentialBuilding::CanFitResidentType(const UUnitType* Type) const
{
    const int Limit = GetResidentTypeLimit(Type);
    return Limit <= 0 || GetResidentTypeCount(Type, true) < Limit;
}

void AResidentialBuilding::SetResidentTypeLimit(UUnitType* Type, int Limit)
{
    ResidentTypeLimits[Type] = Limit;
    OnResidentLimitChanged.Broadcast(Type);
}

TSet<UUnitType*> AResidentialBuilding::GetAllResidentTypes() const
{
    TSet<UUnitType*> Types;
    for (auto Resident : Residents)
    {
        Types.Add(Resident->GetUnitType());
    }
    return Types;
}


