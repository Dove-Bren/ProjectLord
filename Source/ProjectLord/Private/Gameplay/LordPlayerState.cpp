// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordPlayerState.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/Buildings/Building.h"
#include "UI/ViewModels/LordPlayerStateViewModel.h"

ALordPlayerState::ALordPlayerState()
{
    PlayerTeam = EGameTeam::Player1; // Could grab GameInstance and 'claim' next available player team?
    Gold = 0;
}

void ALordPlayerState::BeginPlay()
{
    Super::BeginPlay();

    ViewModel = CreateLordVM<UVMLordPlayerState>(this);
    ViewModel->Setup(this);
}

void ALordPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALordPlayerState, PlayerTeam);
    DOREPLIFETIME(ALordPlayerState, Gold);
    DOREPLIFETIME(ALordPlayerState, PlayerCastle);

}

void ALordPlayerState::SetCastle(ABuilding* Castle)
{
    PlayerCastle = Castle;
}
