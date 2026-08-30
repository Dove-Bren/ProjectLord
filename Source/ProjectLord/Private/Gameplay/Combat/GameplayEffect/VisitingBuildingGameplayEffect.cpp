// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/VisitingBuildingGameplayEffect.h"

#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

#include "Gameplay/LordGameplayTags.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"

UGEVisitingBuilding::UGEVisitingBuilding()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;

    // Add tag
    auto TagsComp = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
    FInheritedTagContainer AppliedTags = TagsComp->GetConfiguredTargetTagChanges(); // Copy what's there
    AppliedTags.AddTag(ULordGameplayTags::UnitStateVisiting());
    AppliedTags.AddTag(ULordGameplayTags::UnitStateInvulnerable()); // ALSO ADDING INVULN
    TagsComp->SetAndApplyTargetTagChanges(AppliedTags);
    GEComponents.Add(TagsComp);

    // Units heal while visiting
    FGameplayModifierInfo Mod;
    Mod.Attribute = UCombatAttributeSet::GetHealthAttribute();
    Mod.ModifierOp = EGameplayModOp::AddBase;
    Mod.ModifierMagnitude = FScalableFloat(5);
    Modifiers.Add(Mod);

    // Heal triggers every second
    Period = 1.0f;
}
