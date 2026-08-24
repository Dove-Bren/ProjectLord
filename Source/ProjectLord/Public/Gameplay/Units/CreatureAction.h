// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "CreatureAction.generated.h"

UENUM(BlueprintType)
enum class ECreatureAction : uint8
{
    Invalid = 0,
    HeroIdle, // "Protecting lands" 
    MonsterIdle, // "is hunting"
    Wandering, // "Looking for adventure"
    InCombat,
    Fleeing, // "is fleeing"
    Dead, // "is dead"

    ExploreQuest, // Explore flag "is doing an exploration quest"
    DefendQuest, // Protect flag, "is protecting lands"
    AttackQuest, // Attack flag, "is fighting" ?

    GoingHome,
    Shopping,

    Patrolling,
    Building,
    CollectingTaxes,
    BringingGold, // Tax collectors
    CarryingGold, // Trade caravans; not sure why different

};
