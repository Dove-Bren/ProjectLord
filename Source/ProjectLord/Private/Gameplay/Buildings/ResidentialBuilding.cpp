// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Buildings/ResidentialBuilding.h"

#include "Gameplay/SelectionComponent.h"
#include "Gameplay/Units/Creature.h"
#include "Gameplay/Units/UnitTypes.h"
#include "UI/ViewModels/Generic/SummarySlotsViewModel.h"

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

ACreature* AResidentialBuilding::RecruitNewUnit(UUnitType* RecruitType, bool bSpawnOutside)
{
    auto World = GetWorld();
    if (!ensure(World))
    {
        return nullptr;
    }

    auto Location = GetBuildingEntrance();
    auto Rotation = FRotator();
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
    ACreature* Recruit = World->SpawnActor<ACreature>(RecruitType->UnitClass, Location, Rotation, Params);
    if (IsValid(Recruit))
    {
        Recruit->SetTeam(GetTeam());
        AddResident(Recruit);
        Recruit->SetHomeBuilding(this);
        Recruit->HandleUnitRecruited();

        if (!bSpawnOutside)
        {
            Recruit->EnterBuilding(this);
        }
    }

    return Recruit;
}

void AResidentialBuilding::SetupSelectionData(USelectionComponent* InSelectionComponent)
{
    Super::SetupSelectionData(InSelectionComponent);

	auto SlotsVM = CreateLordVM<UVMSummarySlots>(this);
	SlotsVM->Init();

    TArray<UVMSummarySlot*> Slots;
    for (auto ResidentType : GetAllResidentTypes())
    {
        UVMSummarySlot* Slot = UVMSummarySlot::MakeUnitTypeCount(this, ResidentType);
        if (const int Limit = GetResidentTypeLimit(ResidentType))
        {
            Slot->SetMaxCount(Limit);
        }
        Slot->SetCount(GetResidentTypeCount(ResidentType, false));
        Slots.Add(Slot);
        OnResidentsChanged.AddWeakLambda(this, [this, ResidentType, Slot]() {
                Slot->SetCount(GetResidentTypeCount(ResidentType, false));
            });
    }

    auto VisitorSlot = UVMSummarySlot::MakeVisitorCount(this);
    Slots.Add(VisitorSlot);
    VisitorSlot->SetCount(GetVisitorCount());
    OnVisitorsChanged.AddWeakLambda(this, [this, VisitorSlot]() {
            VisitorSlot->SetCount(GetVisitorCount());
        });
    
    ensure(Slots.Num() <= 6);
    for (int i = 0; i < Slots.Num() && i < 6; i++)
    {
        SlotsVM->SetSlot(i, Slots[i]);
    }

	InSelectionComponent->SetSlotsVM(SlotsVM);
}

void AResidentialBuilding::HandleDeath()
{
    // TODO: Eject and vacate!
    auto VisitorsCopy = Visitors;
    for (auto& Visitor : VisitorsCopy)
    {
        Visitor->LeaveCurrentBuilding();
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

    // Include types with limits, too
    for (auto& TypeLimit : ResidentTypeLimits)
    {
        Types.Add(TypeLimit.Key);
    }

    return Types;
}


