// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/GenericGameplayTagEffect.h"

#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

UGEGenericGameplayTag::UGEGenericGameplayTag()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    TagContainer = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
    GEComponents.Add(TagContainer);
}

UGEGenericGameplayTag::UGEGenericGameplayTag(FGameplayTag Tag)
    : UGEGenericGameplayTag()
{
    FInheritedTagContainer AppliedTags = TagContainer->GetConfiguredTargetTagChanges(); // Copy what's there
    AppliedTags.AddTag(Tag);
    TagContainer->SetAndApplyTargetTagChanges(AppliedTags);
}
