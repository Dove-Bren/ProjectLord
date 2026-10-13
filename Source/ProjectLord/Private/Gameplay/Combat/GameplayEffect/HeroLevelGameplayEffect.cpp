// Copyright (c) Project Contributors. All Rights Reserved.

#include "Gameplay/Combat/GameplayEffect/HeroLevelGameplayEffect.h"

#include "LordLogging.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "Gameplay/Attributes/CombatAttributeSet.h"

UGEHeroLevel::UGEHeroLevel()
{
    DurationPolicy = EGameplayEffectDurationType::Infinite;
    RefreshModifiers();
}

void UGEHeroLevel::RefreshModifiers()
{
    Modifiers.Empty();

    if (IsValid(ClassAttributeGrowth))
    {
        auto LevelAttribute = UCombatAttributeSet::GetLevelAttribute();
        FString Context = TEXT("ClassAttributeGrowth");
        ClassAttributeGrowth->ForeachRow<FAttributeBaseValue>(Context, [this, LevelAttribute](const FName& Key, const FAttributeBaseValue& Value)
            {
                double AttributeValue = Value.BaseValue;
                if (Value.Variation > 0)
                {
                    UE_LOG(LordCombatAbility, Error, TEXT("Found a non-zero variation for attribute in row name [%s]. Ignoring"), *Key.ToString());
                }

                FGameplayModifierInfo Mod;
                Mod.Attribute = Value.Attribute;
                Mod.ModifierOp = EGameplayModOp::AddBase;
                FAttributeBasedFloat Curve;
                Curve.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(LevelAttribute, EGameplayEffectAttributeCaptureSource::Source, false);
                Curve.Coefficient = FMath::Floor(AttributeValue);
                Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Curve);
                Modifiers.Add(MoveTemp(Mod));
            });
    }
}

#if WITH_EDITOR
void UGEHeroLevel::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    FName PropertyName = (PropertyChangedEvent.Property != nullptr) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(ThisClass, ClassAttributeGrowth))
    {
        RefreshModifiers();
    }
}
#endif // WITH_EDITOR  
