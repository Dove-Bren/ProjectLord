// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/GenericGameplayTagEffect.h"

#include "GameplayEffectComponents/TargetTagsGameplayEffectComponent.h"

#include "Gameplay/LordGameplayTags.h"

UGEGenericGameplayTag::UGEGenericGameplayTag()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    TagContainer = CreateDefaultSubobject<UTargetTagsGameplayEffectComponent>(TEXT("TargetTagsComponent"));
}

void UGEGenericGameplayTag::AddTag(FGameplayTag Tag)
{
    FInheritedTagContainer AppliedTags = TagContainer->GetConfiguredTargetTagChanges(); // Copy what's there
    AppliedTags.AddTag(Tag);
    TagContainer->SetAndApplyTargetTagChanges(AppliedTags);
}
