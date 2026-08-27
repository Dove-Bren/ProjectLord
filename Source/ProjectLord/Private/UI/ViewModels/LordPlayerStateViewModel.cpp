// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/LordPlayerStateViewModel.h"

#include "Gameplay/LordPlayerState.h"

UVMLordPlayerState::UVMLordPlayerState()
{
	
}

void UVMLordPlayerState::Setup(ALordPlayerState* State)
{
	ParentState = State;

	SetTeam(State->GetPlayerTeam());
}
