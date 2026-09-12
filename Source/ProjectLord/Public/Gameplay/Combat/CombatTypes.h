// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CombatTypes.generated.h"

UENUM(BlueprintType)
enum class EDamageType : uint8
{
    System,
    Melee,
    Ranged,
    Magic,
};

USTRUCT(BlueprintType)
struct FDamageTypeMap
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType Map")
    bool bMelee = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType Map")
    bool bRanged = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DamageType Map")
    bool bMagic = false;

    void Merge(const FDamageTypeMap& Other)
    {
        bMelee |= Other.bMelee;
        bRanged |= Other.bRanged;
        bMagic |= Other.bMagic;
    }

    bool operator==(const FDamageTypeMap& Other) const
    {
        return Other.bMelee == bMelee
            && Other.bRanged == bRanged
            && Other.bMagic == bMagic
            ;

    }
};
