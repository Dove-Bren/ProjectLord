// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/SelfRepairGameplayEffect.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"

UGESelfRepair::UGESelfRepair()
{
    FGameplayModifierInfo Mod;
    Mod.Attribute = UCombatAttributeSet::GetHealthAttribute();
    Mod.ModifierOp = EGameplayModOp::AddBase;
    FSetByCallerFloat Magnitude; Magnitude.DataName = AmountName;
    Mod.ModifierMagnitude = Magnitude;
    Modifiers.Add(Mod);
}

FName UGESelfRepair::AmountName = TEXT("SelfRepairAmount");
