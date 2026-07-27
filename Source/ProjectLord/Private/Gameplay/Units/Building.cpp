// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/Building.h"

ABuilding::ABuilding()
{
	
}

bool ABuilding::RemoveResident(const ACreature* Resident)
{
	return Residents.RemoveAll([Resident](ACreature* const InResident) { return Resident == InResident; }) > 0;
}

void ABuilding::RemoveAllResidents()
{
	Residents.Empty();
}

bool ABuilding::AddResident(ACreature* Resident)
{
	// TODO: Verify there is space
	Residents.Add(Resident);
	return true;
}

void ABuilding::RemoveAllVisitors()
{
	Visitors.Empty();
}

void ABuilding::AddVisitor(ACreature* Visitor)
{
	Visitors.Add(Visitor);
}

void ABuilding::RemoveVisitor(const ACreature* Visitor)
{
	Visitors.RemoveAll([Visitor](ACreature* const InVisitor) { return Visitor == InVisitor; });
}

FVector ABuilding::GetBuildingEntrance() const
{
	return GetActorLocation() + BuildingEntranceOffset;
}
