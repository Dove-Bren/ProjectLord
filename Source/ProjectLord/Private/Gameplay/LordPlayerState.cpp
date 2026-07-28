// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordPlayerState.h"

#include "Net/UnrealNetwork.h"

#include "Gameplay/Units/Building.h"

ALordPlayerState::ALordPlayerState()
{
    PlayerTeam = EUnitTeam::Player1; // Could grab GameInstance and 'claim' next available player team?
    Gold = 0;
}

void ALordPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ALordPlayerState, PlayerTeam);
    DOREPLIFETIME(ALordPlayerState, Gold);
    DOREPLIFETIME(ALordPlayerState, PlayerCastle);

}
