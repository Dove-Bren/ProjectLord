// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "Gameplay/Units/HeroBase.h"

AHeroBase::AHeroBase() : AUnit()
{
	Team = EUnitTeam::Player1;
	Gold = 0;
	HeroXP = 0;
}

int AHeroBase::GetHeroMaxXP() const
{
	return 100 + Level * 20;
}
