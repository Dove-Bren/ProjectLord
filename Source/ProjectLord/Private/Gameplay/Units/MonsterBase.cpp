// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/MonsterBase.h"

AMonsterBase::AMonsterBase() : ACreature()
{
	Team = EUnitTeam::Monster;
	Gold = 5;
}
