// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    System,
    Slash,
    Pierce,
    Magic,
};

USTRUCT(BlueprintType)
struct PROJECTLORD_API FDefenses
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int SlashDefense;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int PierceDefense;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int MagicDefense;

    int GetDefense(EDamageType InType)
    {
        switch (InType)
        {
        case EDamageType::Slash:
            return SlashDefense;
        case EDamageType::Pierce:
            return PierceDefense;
        case EDamageType::Magic:
            return MagicDefense;
        }

        return 0;
    }
};

USTRUCT(BlueprintType)
struct PROJECTLORD_API FAttackDamage
{
    GENERATED_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDamageType DamageType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0))
    int Amount;
};
