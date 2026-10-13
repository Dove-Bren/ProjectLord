// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Combat/GameplayEffect/GenericGameplayTagEffect.h"

#include "UntargetableGameplayEffect.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UGEUntargetable : public UGEGenericGameplayTag
{
    GENERATED_BODY()

public:

    UGEUntargetable();
};
