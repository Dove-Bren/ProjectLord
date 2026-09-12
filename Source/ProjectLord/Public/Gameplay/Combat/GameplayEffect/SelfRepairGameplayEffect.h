// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "SelfRepairGameplayEffect.generated.h"

UCLASS()
class PROJECTLORD_API UGESelfRepair : public UGameplayEffect
{
    GENERATED_BODY()

public:
    
    UGESelfRepair();

    static FName AmountName;
};
