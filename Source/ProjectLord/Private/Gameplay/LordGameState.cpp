// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordGameState.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

#include "UI/ViewModels/LordGameStateViewModel.h"

ALordGameState::ALordGameState()
{
	GameSpeed = 1.0f;
	GameDays = 0;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ALordGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALordGameState, GameDays);
	DOREPLIFETIME(ALordGameState, GameSpeed);
	DOREPLIFETIME(ALordGameState, GameTeamsArray);
}

void ALordGameState::BeginPlay()
{
	Super::BeginPlay();

	ViewModel = CreateLordVM<UVMLordGameState>(this);
	ViewModel->Setup(this);

	for (EGameTeam Team : {EGameTeam::Monster, EGameTeam::Neutral, EGameTeam::Player1, EGameTeam::Player2})
	{
		auto TeamState = AGameTeamState::Make(this, Team);
		GameTeams.Add(Team, TeamState);
		GameTeamsArray.Add(TeamState);
	}
}

void ALordGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Advance game time
	{
		constexpr double SECONDS_PER_GAME_DAY = 60 * 1;
		const double OldTime = GameDays;
		GameDays += (DeltaTime / SECONDS_PER_GAME_DAY);
		OnRep_GameDays(OldTime);
	}
}

void ALordGameState::SetGameSpeed(float InSpeed)
{
	const float OldSpeed = GameSpeed;
	GameSpeed = InSpeed;
	OnRep_GameSpeed(OldSpeed);
}

void ALordGameState::OnRep_GameDays(double PrevGameDays)
{
	const int OldDays = FMath::FloorToInt(PrevGameDays);
	const int NewDays = FMath::FloorToInt(GameDays);
	if (OldDays != NewDays)
	{
		OnGameDayChange.Broadcast(NewDays);
	}
	OnGameTimeChange.Broadcast(GameDays);
}

void ALordGameState::OnRep_GameSpeed(float PrevGameSpeed)
{
	if (PrevGameSpeed != GameSpeed)
	{
		SetGlobalTimeDilation(GameSpeed);
		OnGameSpeedChange.Broadcast(GameSpeed);
	}
}

void ALordGameState::OnRep_GameTeams(TArray<AGameTeamState*> PrevTeams)
{
	// Fix up map
	for (auto Team : GameTeamsArray)
	{
		GameTeams[Team->GetTeam()] = Team;
	}
}

void ALordGameState::SetGlobalTimeDilation(float Rate)
{
	UGameplayStatics::SetGlobalTimeDilation(this, Rate);
}
