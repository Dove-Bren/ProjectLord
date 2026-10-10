// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordGameMode.h"

#include "Gameplay/LordGameState.h"
#include "Gameplay/GameTeamState.h"
#include "Gameplay/Buildings/Castle.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Player/LordPlayerState.h"

ALordGameMode::ALordGameMode()
{
}

void ALordGameMode::PostLogin(APlayerController* NewController)
{
	Super::PostLogin(NewController);
}

FString ALordGameMode::InitNewPlayer(APlayerController* NewPlayerController, const FUniqueNetIdRepl& UniqueId, const FString& Options, const FString& Portal)
{
	// Try to set up for this player with the game instance
	auto State = GetGameState<ALordGameState>();
	if (ensure(State))
	{
		// Attempt to claim and spawn player team before
		// controller is created so that its beginplay can assume it's there.
		auto Claim = State->ClaimNextPlayerTeam();
		if (ensure(Claim)) // ensure for now, because spectator isn't really set up
		{
			State->SpawnTeamCastle(Claim.GetValue(), GetCastleForPlayer(NewPlayerController));
			auto LordController = Cast<ALordPlayerController>(NewPlayerController);
			LordController->GetLordPlayerState()->SetPlayerTeam(Claim->Team);
		}
		else
		{
			NewPlayerController->StartSpectatingOnly();
		}
	}

	// Call super AFTER we set up teams, since super will pick player starts and we want
	// to have created a team by then
	return Super::InitNewPlayer(NewPlayerController, UniqueId, Options, Portal);
}

TSubclassOf<ACastle> ALordGameMode::GetCastleForPlayer(const APlayerController* Controller) const
{
	return PlayerCastleType;
}

AActor* ALordGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	if (auto LordPC = Cast<ALordPlayerController>(Player))
	{
		auto State = GetGameState<ALordGameState>();
		if (ensure(State))
		{
			auto TeamState = State->GetTeam(LordPC->GetTeam());
			if (ensure(TeamState) && ensure(TeamState->GetCastle()))
			{
				return TeamState->GetCastle();
			}
		}
	}

	return Super::ChoosePlayerStart_Implementation(Player);
}

void ALordGameMode::FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation)
{
	// Adjust for the fact that our buildings are all misaligned
	FRotator Rot = StartRotation; // Copy
	Rot.Yaw -= 90;

	Super::FinishRestartPlayer(NewPlayer, Rot);
}
