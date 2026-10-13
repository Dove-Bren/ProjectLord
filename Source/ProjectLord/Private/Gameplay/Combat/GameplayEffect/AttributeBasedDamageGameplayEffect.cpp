// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/AttributeBasedDamageGameplayEffect.h"

#include "Gameplay/Attributes/CombatAttributeSet.h"
#include "Gameplay/Attributes/LordHeroAttributeSet.h"

float UGEAttributeBasedDamage::DamageModPerAttribute = (1.0f / 3.0f);

void UGEAttributeBasedDamage::Setup(FGameplayAttribute Source, FGameplayAttribute Damage)
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    FGameplayModifierInfo Mod;
    Mod.Attribute = Damage;
    Mod.ModifierOp = EGameplayModOp::AddFinal;
    FAttributeBasedFloat Curve;
    Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(Source, EGameplayEffectAttributeCaptureSource::Source, false);
    Curve.Coefficient = DamageModPerAttribute;
    Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
    Modifiers.Add(MoveTemp(Mod));
}

UGEAttributeBasedDamage::UGEAttributeBasedDamage()
{

}

UGEStrengthBasedMeleeDamage::UGEStrengthBasedMeleeDamage()
{
    Setup(ULordHeroAttributeSet::GetStrengthAttribute(), UCombatAttributeSet::GetMeleeDamageAttribute());
}

UGEAgilityBasedRangedDamage::UGEAgilityBasedRangedDamage()
{
    Setup(ULordHeroAttributeSet::GetAgilityAttribute(), UCombatAttributeSet::GetRangedDamageAttribute());
}

UGEIntelligenceBasedMagicDamage::UGEIntelligenceBasedMagicDamage()
{
    Setup(ULordHeroAttributeSet::GetIntelligenceAttribute(), UCombatAttributeSet::GetMagicDamageAttribute());
}
