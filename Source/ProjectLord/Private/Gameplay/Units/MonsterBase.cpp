// Copyright (c) Skyler Manzanares. All Rights Reserved.

#include "Gameplay/Units/MonsterBase.h"

AMonsterBase::AMonsterBase() : AUnit()
{
	Team = EUnitTeam::Monster;
	Gold = 5;
}
