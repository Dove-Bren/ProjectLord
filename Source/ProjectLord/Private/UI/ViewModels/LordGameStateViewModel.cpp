// Copyright (c) Project Contributors. All Rights Reserved.

#include "UI/ViewModels/LordGameStateViewModel.h"

#include "Gameplay/LordGameState.h"

UVMLordGameState::UVMLordGameState()
{
}

void UVMLordGameState::Setup(class ALordGameState* State)
{
	ParentState = State;
	State->OnGameTimeChange.AddDynamic(this, &UVMLordGameState::HandleGameTimeChanged);
	State->OnGameSpeedChange.AddDynamic(this, &UVMLordGameState::HandleGameSpeedChanged);
	SetGameTime(State->GetGameDays());
	SetGameSpeed(State->GetGameSpeed());
}

void UVMLordGameState::HandleGameTimeChanged(double InGameDay)
{
	SetGameTime(InGameDay);
}

void UVMLordGameState::HandleGameSpeedChanged(float InGameSpeed)
{
	SetGameSpeed(InGameSpeed);
}

void UVMLordGameState::ModifyGameSpeed(float InSpeed)
{
	if (ensure(ParentState.IsValid()))
	{
		ParentState->SetGameSpeed(InSpeed);
	}
}

void UVMLordGameState::SetGamePaused(bool bPaused)
{
	if (ensure(ParentState.IsValid()))
	{
		ParentState->SetGamePaused(bPaused);
	}
}
