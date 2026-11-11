// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/AI/MonsterTeamController.h"

#include "Gameplay/Map/GameLevelSettings.h"
#include "Gameplay/Map/Map.h"
#include "Gameplay/GameTeamState.h"
#include "Gameplay/LordGameState.h"
#include "Gameplay/AI/UnitController.h"
#include "Gameplay/Buildings/Building.h"
#include "Gameplay/Buildings/Castle.h"
#include "Gameplay/Buildings/SpawningBuilding.h"
#include "Gameplay/Units/UnitTypes.h"
#include "Gameplay/Units/Creature.h"

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

ABuilding* AMonsterTeamController::GetNearestPlayerBuilding(FVector From) const
{
	auto GameState = GetWorld()->GetGameState<ALordGameState>();
	if (!ensure(GameState))
	{
		return nullptr;
	}

	ABuilding* Closest = nullptr;
	double ClosestDistSqr = DOUBLE_BIG_NUMBER;

	for (auto PlayerTeam : { EGameTeam::Player1, EGameTeam::Player2 })
	{
		auto PlayerTeamState = GameState->GetTeam(PlayerTeam);
		if (PlayerTeamState)
		{
			for (auto Building : PlayerTeamState->GetBuildings())
			{
				if (!Building) continue;

				const double DistSqr = FVector::DistSquaredXY(Building->GetActorLocation(), From);
				if (!Closest || DistSqr < ClosestDistSqr)
				{
					Closest = Building;
					ClosestDistSqr = DistSqr;
				}
			}
		}
	}

	return Closest;
}

void AMonsterTeamController::HandleDayChange(int GameDays)
{
	ActivateSpawners(GameDays);
	DoEdgeSpawn(GameDays);
}

void AMonsterTeamController::ActivateSpawners(int GameDays)
{
	auto GameState = GetWorld()->GetGameState<ALordGameState>();
	if (!ensure(GameState))
	{
		return;
	}

	// Activate spawners that are now in range
	auto Settings = GameState->GetLevelSettings();
	if (!ensure(Settings))
	{
		return;
	}

	// Check grace period early
	if (GameDays < Settings->GetMonsterGracePeriod())
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

	const int DaysToUse = (GameDays + 1) - Settings->GetMonsterGracePeriod();
	const double DayRangeSqr = FMath::Square(Settings->GetMonsterActivationPerDay() * DaysToUse);

	for (ABuilding* Building : TeamState->GetBuildings())
	{
		if (!Building) continue;

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

void AMonsterTeamController::DoEdgeSpawn(int GameDays)
{
	auto GameState = GetWorld()->GetGameState<ALordGameState>();
	if (!ensure(GameState))
	{
		return;
	}
	auto Settings = GameState->GetLevelSettings();
	if (!ensure(Settings))
	{
		return;
	}

	// Check grace period early
	if (GameDays < Settings->GetMonsterGracePeriod())
	{
		return;
	}

	const auto SwarmChance = Settings->GetMonsterSwarmChance();
	int SpawnCount = Settings->GetMonsterSwarmCount();
	if (SwarmChance <= 0 || SpawnCount <= 0)
	{
		return;
	}

	// Should have at least one spawn type
	auto SpawnTypes = Settings->GetMonsterSwarmTypes();
	if (!ensure(!SpawnTypes.IsEmpty()))
	{
		return;
	}

	// Need map, too
	auto Map = AMap::GetMap(this);
	if (!ensure(Map))
	{
		return;
	}

	// Roll and early out if fail
	if (SwarmChance < 1.0f && FMath::FRand() >= SwarmChance)
	{
		return;
	}

	const auto MinPoint = Map->GetMapMinPoint();
	while (SpawnCount-- > 0)
	{
		// Roll random spot to spawn on edge
		const auto W = FMath::FRand();
		const bool bVert = FMath::RandBool();
		const bool bMax = FMath::RandBool();
		float DX;
		float DY;
		if (bVert)
		{
			// random value is the Y and bmax switches between 0 and 1 x
			DY = W;
			DX = bMax ? 0.95f : 0.05f;
		}
		else
		{
			DX = W;
			DY = bMax ? 0.95f : 0.05f;
		}

		float X = MinPoint.X + Map->GetMapWidth() * DX;
		float Y = MinPoint.Y + Map->GetMapHeight() * DY;
		float Z;

		// Discover height (Z)
		FHitResult HitResult;
		if (GetWorld()->LineTraceSingleByChannel(HitResult, FVector(X, Y, 10000), FVector(X, Y, -10000), ECollisionChannel::ECC_Camera))
		{
			Z = HitResult.Location.Z;
		}
		else
		{
			Z = 0;
		}

		// Roll spawned type
		const auto MonsterType = SpawnTypes.Num() == 1 ? SpawnTypes[0] : SpawnTypes[FMath::RandRange(0, SpawnTypes.Num() - 1)];

		// Do spawn
		auto Location = FVector(X, Y, Z);
		auto Rotation = FRotator();
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		ACreature* Monster = GetWorld()->SpawnActor<ACreature>(MonsterType->UnitClass, Location, Rotation, Params);
		if (ensure(Monster))
		{
			Monster->Server_SetTeam(GetTeam());
			// TODO always do this?
			auto Building = GetNearestPlayerBuilding(Location);
			if (Building)
			{
				Monster->GetUnitController()->OverrideTarget(Building->GetCombatComponent());
			}
		}
	}
	
}
