// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "DamageFunctionLibrary.generated.h"

UCLASS()
class PROJECTLORD_API UDamageFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static int CalculateBaseMeleeDamageDirect(int HeroLevel, int Strength, int BonusDamage);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static int CalculateBaseRangedDamageDirect(int HeroLevel, int Agility, int BonusDamage);

    UFUNCTION(BlueprintPure, Category = "Combat|Damage")
    static int CalculateBaseMagicDamageDirect(int HeroLevel, int Intelligence, int BonusDamage);

};
