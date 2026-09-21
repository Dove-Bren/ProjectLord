// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/AI/MonsterTeamController.h"

#include "Gameplay/GameLevelSettings.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/Castle.h"
#include "Gameplay/Buildings/SpawningBuilding.h"

AMonsterTeamController::AMonsterTeamController()
{

}

void AMonsterTeamController::BeginPlay()
{
	Super::BeginPlay();

	auto GameState = GetWorld()->GetGameState<ALordGameState>();
	if (ensure(GameState))
	{
		GameState->OnGameDayChange.AddDynamic(this, &ThisClass::HandleDayChange);
	}

	auto TeamState = GetTeamState();
	if (ensure(TeamState))
	{
		TeamState->SetPrimaryMonsterController(this);
	}
}

void AMonsterTeamController::EndPlay(EEndPlayReason::Type Reason)
{
	Super::EndPlay(Reason);

}

void AMonsterTeamController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AGameTeamState* AMonsterTeamController::GetTeamState() const
{
	if (auto State = GetWorld()->GetGameState<ALordGameState>())
	{
		return State->GetTeam(GetTeam());
	}
	return nullptr;
}

void AMonsterTeamController::HandleDayChange(int GameDays)
{
	ActivateSpawners(GameDays);
}

void AMonsterTeamController::ActivateSpawners(int GameDays)
{
	auto GameState = GetWorld()->GetGameState<ALordGameState>();
	if (!ensure(GameState))
	{
		return;
	}

	auto TeamState = GetTeamState();
	if (!ensure(TeamState))
	{
		return;
	}

	// Get a list of all player palaces
	TArray<const ACastle*> Palaces;
	for (auto PlayerTeam : { EGameTeam::Player1, EGameTeam::Player2 })
	{
		auto PlayerTeamState = GameState->GetTeam(PlayerTeam);
		if (PlayerTeamState)
		{
			auto Palace = PlayerTeamState->GetCastle();
			if (Palace) Palaces.Add(Palace);
		}
	}

	if (Palaces.IsEmpty())
	{
		// nothing to do!
		return;
	}

	// Activate spawners that are now in range
	auto Settings = GameState->GetLevelSettings();
	const double DayRangeSqr = FMath::Square(Settings->GetMonsterActivationPerDay() * (GameDays + 1));

	for (ABuilding* Building : TeamState->GetBuildings())
	{
		if (auto Spawner = Cast<ASpawningBuilding>(Building))
		{
			if (Spawner->IsActivated())
			{
				continue;
			}

			// See if it's within range of any palaces
			for (const auto Palace : Palaces)
			{
				if (Palace->GetSquaredHorizontalDistanceTo(Spawner) <= DayRangeSqr)
				{
					Spawner->ActivateSpawner();
				}
			}
		}
	}
}
