// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/LevelBasedConfidenceGameplayEffect.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/LordHeroAttributeSet.h"

UGELevelBasedConfidence::UGELevelBasedConfidence()
{
    auto ConfidenceAttribute = ULordHeroAttributeSet::GetConfidenceAttribute();
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    FGameplayModifierInfo Mod;
    Mod.Attribute = ConfidenceAttribute;
    Mod.ModifierOp = EGameplayModOp::AddBase;
    FAttributeBasedFloat Curve;
    Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(UCombatAttributeSet::GetLevelAttribute(), EGameplayEffectAttributeCaptureSource::Source, false);
    Curve.Coefficient = 1;
    Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
    Modifiers.Add(MoveTemp(Mod));
}
