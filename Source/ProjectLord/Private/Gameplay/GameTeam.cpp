// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/GameTeam.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Units/Unit.h"
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

int AGameTeamState::AddGold(int InChange)
{
    Gold = FMath::Clamp(Gold + InChange, 0, 9999999);
    OnTeamGoldChanged.Broadcast(Gold);
    return Gold;
}

void AGameTeamState::SetCastle(ABuilding* Castle)
{
    TeamCastle = Castle;
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

void AGameTeamState::OnUnitFinalDeath(AUnit* Unit)
{
    RemoveUnit(Unit);
}