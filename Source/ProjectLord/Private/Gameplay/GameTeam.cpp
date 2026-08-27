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

    DOREPLIFETIME(AGameTeamState, Team);
    DOREPLIFETIME(AGameTeamState, Gold);
    DOREPLIFETIME(AGameTeamState, TeamCastle);
    DOREPLIFETIME(AGameTeamState, TeamUnits);
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
    TeamUnits.Add(Unit);
    Unit->OnUnitFinalDeath.AddDynamic(this, &AGameTeamState::OnUnitFinalDeath);
    OnTeamUnitsChanged.Broadcast();
}

void AGameTeamState::RemoveUnit(AUnit* Unit)
{
    if (TeamUnits.Remove(Unit))
    {
        OnTeamUnitsChanged.Broadcast();
        Unit->OnUnitFinalDeath.RemoveAll(this);
    }
}

void AGameTeamState::OnUnitFinalDeath(AUnit* Unit)
{
    RemoveUnit(Unit);
}