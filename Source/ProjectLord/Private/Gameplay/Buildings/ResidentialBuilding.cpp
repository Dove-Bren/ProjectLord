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

bool AResidentialBuilding::RemoveResident(const ACreature* Resident)
{
	return Residents.RemoveAll([Resident](ACreature* const InResident) { return Resident == InResident; }) > 0;
}

void AResidentialBuilding::RemoveAllResidents()
{
	Residents.Empty();
}

bool AResidentialBuilding::AddResident(ACreature* Resident)
{
	// TODO: Verify there is space
	Residents.Add(Resident);
	return true;
}

void AResidentialBuilding::RemoveAllVisitors()
{
	Visitors.Empty();
}

void AResidentialBuilding::AddVisitor(ACreature* Visitor)
{
	Visitors.Add(Visitor);
}

void AResidentialBuilding::RemoveVisitor(const ACreature* Visitor)
{
	Visitors.RemoveAll([Visitor](ACreature* const InVisitor) { return Visitor == InVisitor; });
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


