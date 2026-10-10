// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordGameState.h"

#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

#include "Gameplay/Buildings/Castle.h"
#include "Gameplay/Map/GameLevelSettings.h"
#include "Gameplay/Map/Map.h"
#include "Gameplay/GameTeamState.h"
#include "UI/ViewModels/LordGameStateViewModel.h"

ALordGameState::ALordGameState()
{
	GameSpeed = 1.0f;
	GameDays = 0;
	bStartComplete = false;
	bStartReceived = false;
	bTeamsSetup = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	bReplicates = true;
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

	// Dedicated server not supported; lots of stuff will unconditionally make UI and view models.
	check(GetNetMode() != ENetMode::NM_DedicatedServer);

	ViewModel = CreateLordVM<UVMLordGameState>(this);
	ViewModel->Setup(this);

	if (HasAuthority())
	{
		bStartReceived = true;
		SetupTeams();
	}
	// else will finish setup once replicated
}

void ALordGameState::PostNetInit()
{
	Super::PostNetInit();
	bStartReceived = true;
}

void ALordGameState::SetupTeams()
{
	if (bTeamsSetup)
	{
		return;
	}
	bTeamsSetup = true;

	// Find game settings, or make default
	{
		for (TActorIterator<AGameLevelSettings> It(GetWorld()); It; ++It)
		{
			LevelSettings = *It;
			break;
		}

		if (!LevelSettings)
		{
			FActorSpawnParameters Params;
			Params.bNoFail = true;
			Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			LevelSettings = GetWorld()->SpawnActor<AGameLevelSettings>();
			check(LevelSettings);
		}
	}

	for (EGameTeam Team : {EGameTeam::Monster, EGameTeam::Neutral})
	{
		auto TeamState = AGameTeamState::Make(this, Team);
		GameTeams.Add(Team, TeamState);
		GameTeamsArray.Add(TeamState);
	}
}

void ALordGameState::CheckTeamSetup()
{
	check(!bTeamsSetup);

	// Non-replicated teams show up as null in the replicated teams array.
	// So iterate it and see if there aren't any more nulls (all have been resolved)
	for (auto Team : GameTeamsArray)
	{
		if (!Team)
		{
			return;
		}
	}

	// No nulls detected
	bTeamsSetup = true;
}

void ALordGameState::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bStartComplete)
	{
		if (!bTeamsSetup && !HasAuthority())
		{
			// Clients tick and wait for teams to finish replicating
			CheckTeamSetup();
		}

		if (bStartReceived && bTeamsSetup)
		{
			bStartComplete = true;
			OnGameStateReady.Complete(true);
		}
		else
		{
			return;
		}
	}

	// begin normal tick

	// server tick
	if (HasAuthority())
	{
		// Advance game time
		{
			constexpr double SECONDS_PER_GAME_DAY = 60 * 1;
			const double OldTime = GameDays;
			GameDays += (DeltaTime / SECONDS_PER_GAME_DAY);
			OnRep_GameDays(OldTime);
		}
	}
}

void ALordGameState::SetGameSpeed(float InSpeed)
{
	const float OldSpeed = GameSpeed;
	GameSpeed = InSpeed;
	OnRep_GameSpeed(OldSpeed);
}

void ALordGameState::SetGamePaused(bool bPaused)
{
	UGameplayStatics::SetGamePaused(this, bPaused);
}

AGameTeamState* ALordGameState::GetTeam(EGameTeam Team)
{
	//Setup();
	return Team < EGameTeam::MAX ? GameTeams.FindRef(Team) : nullptr;
}

TArray<FTransform> ALordGameState::GetPlayerStarts() const
{
	TArray<FTransform> StartLocations;
	auto Map = AMap::GetMap(this);
	if (ensure(Map))
	{
		for (auto Start : Map->FindPlayerStarts())
		{
			auto Transform = Start->GetActorTransform();
			// For better or worse, player starts need +90 yaw for buildings
			auto Rot = Transform.Rotator();
			Rot.Yaw += 90;
			Transform.SetRotation(Rot.Quaternion());
			StartLocations.Add(Transform);
		}
	}

	if (StartLocations.IsEmpty())
	{
		StartLocations.Add({}); // Use 0,0,0
	}
	return StartLocations;
}

TOptional<FTeamStartClaim> ALordGameState::ClaimNextPlayerTeam()
{
	// Will just lazily discover what the next available player is each time.
	// First, find what team is left
	TOptional<EGameTeam> NextTeam = NullOpt;
	for (auto PlayerTeam : { EGameTeam::Player1, EGameTeam::Player2 })
	{
		auto TeamState = GetTeam(PlayerTeam);
		if (TeamState)
		{
			continue;
		}

		NextTeam = PlayerTeam;
		break;
	}

	if (!NextTeam.IsSet())
	{
		return NullOpt;
	}

	// Next, get list of player starts from map.
	TArray<FTransform> StartLocations = GetPlayerStarts();
	
	// Filter out any that are too close to an existing castle
	StartLocations.RemoveAll([this](const FTransform& Location) -> bool {
		auto At = Location.GetLocation();
		for (const auto Team : GameTeamsArray)
		{
			auto CenterOpt = Team->GetTeamCenterLocation();
			if (CenterOpt.IsSet() && FVector::DistSquaredXY(CenterOpt.GetValue(), At) < 1000 * 1000)
			{
				return true; // remove
			}
		}
		return false; // none matched
	});

	// If no more start locations, can't claim team
	if (StartLocations.IsEmpty())
	{
		return NullOpt;
	}

	// Use first location.
	// TODO: use whichever has the biggest distance to any other center location?
	FTransform Transform = StartLocations[0];

	// Claim by creating team state
	auto TeamState = AGameTeamState::Make(this, NextTeam.GetValue(), Transform.GetLocation());
	GameTeams.Add(NextTeam.GetValue(), TeamState);
	GameTeamsArray.Add(TeamState);
	
	FTeamStartClaim Claim;
	Claim.StartTransform = Transform;
	Claim.Team = NextTeam.GetValue();
	return Claim;
}

ACastle* ALordGameState::SpawnTeamCastle(const FTeamStartClaim& InClaim, TSubclassOf<ACastle> CastleType)
{
	// Check if team already has castle
	auto TeamState = GetTeam(InClaim.Team);
	auto Castle = TeamState->GetCastle();
	if (IsValid(Castle))
	{
		return Castle;
	}

	// Spawn it
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		Castle = GetWorld()->SpawnActor<ACastle>(CastleType, InClaim.StartTransform, Params);

		if (!ensure(Castle))
		{
			return nullptr;
		}

		Castle->Server_SetTeam(InClaim.Team);
		Castle->SetSpawnTeam(InClaim.Team);
		Castle->ClearFoliageAround();
		Castle->PaintFoundation();
	}

	TeamState->SetCastle(Castle);
	return Castle;
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
		if (!Team) continue; // Array can replicate out of order
		GameTeams.Add(Team->GetTeam(), Team);
	}
}

void ALordGameState::SetGlobalTimeDilation(float Rate)
{
	UGameplayStatics::SetGlobalTimeDilation(this, Rate);
}
