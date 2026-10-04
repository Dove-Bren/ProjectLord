// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/LordGameMode.h"

#include "Gameplay/LordGameState.h"
#include "Gameplay/Player/LordPlayerController.h"
#include "Gameplay/Player/LordPlayerState.h"

ALordGameMode::ALordGameMode()
{
}

void ALordGameMode::PostLogin(APlayerController* NewController)
{
	Super::PostLogin(NewController);

	// Try to set up for this player with the game instance
	auto State = GetGameState<ALordGameState>();
	if (!ensure(State))
	{
		return;
	}

	// Attempt to claim and spawn player team before
	// controller is created so that its beginplay can assume it's there.
	auto Claim = State->ClaimNextPlayerTeam();
	if (Claim)
	{
		State->SpawnTeamCastle(Claim.GetValue(), GetCastleForPlayer(NewController));
		auto LordController = Cast<ALordPlayerController>(NewController);
		LordController->GetLordPlayerState()->SetPlayerTeam(Claim->Team);
	}
	else
	{
		NewController->StartSpectatingOnly();
	}
}

TSubclassOf<ACastle> ALordGameMode::GetCastleForPlayer(const APlayerController* Controller) const
{
	return PlayerCastleType;
}
