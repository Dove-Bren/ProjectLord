// Copyright (c) Project Contributors. All Rights Reserved.

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
    static int CalculateDamage(int AttackStrength, int Defense);

};
