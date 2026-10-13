// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "AttributeBasedDamageGameplayEffect.generated.h"

UCLASS(Abstract)
class PROJECTLORD_API UGEAttributeBasedDamage : public UGameplayEffect
{
    GENERATED_BODY()

public:
    UGEAttributeBasedDamage();

protected:
    void Setup(FGameplayAttribute Source, FGameplayAttribute Damage);

    static float DamageModPerAttribute;
};

UCLASS()
class PROJECTLORD_API UGEStrengthBasedMeleeDamage : public UGEAttributeBasedDamage
{
    GENERATED_BODY()

public:
    UGEStrengthBasedMeleeDamage();
};

UCLASS()
class PROJECTLORD_API UGEAgilityBasedRangedDamage : public UGEAttributeBasedDamage
{
    GENERATED_BODY()

public:
    UGEAgilityBasedRangedDamage();
};

UCLASS()
class PROJECTLORD_API UGEIntelligenceBasedMagicDamage : public UGEAttributeBasedDamage
{
    GENERATED_BODY()

public:
    UGEIntelligenceBasedMagicDamage();
};
