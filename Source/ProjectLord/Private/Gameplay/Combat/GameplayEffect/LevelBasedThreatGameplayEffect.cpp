// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/LevelBasedThreatGameplayEffect.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"

UGELevelBasedThreat::UGELevelBasedThreat()
{
    auto ThreatAttribute = UCombatAttributeSet::GetThreatAttribute();
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    FGameplayModifierInfo Mod;
    Mod.Attribute = ThreatAttribute;
    Mod.ModifierOp = EGameplayModOp::AddBase;
    FAttributeBasedFloat Curve;
    Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(UCombatAttributeSet::GetLevelAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
    Curve.Coefficient = 1;
    Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
    Modifiers.Add(MoveTemp(Mod));
}
