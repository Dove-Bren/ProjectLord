// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/MonsterBase.h"

AMonsterBase::AMonsterBase() : ACreature()
{
	Team = EGameTeam::Monster;
	Gold = 5;
}

void AMonsterBase::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	// If we have any gold
	if (Gold)
	{
		int Leftover = 0;
		int Distribute = Gold;
		if (Gold >= 25) // Not sure how this is actually decided in game
		{
			Leftover = FMath::FloorToInt((float)Gold / 5.0f);
			Distribute = Gold - Leftover;
		}

		// Award gold, noting if we didn't hand out any
		if (!AwardGoldToNearbyHeroes(Distribute))
		{
			// If none was awarded, put it all back in leftover
			Leftover = Gold;
		}

		// Remember how much we have leftover for our grave
		SetGold(Leftover);
	}
}
