// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Units/MonsterBase.h"

AMonsterBase::AMonsterBase() : ACreature()
{
	Team = EGameTeam::Monster;
	Gold = 5;
}
