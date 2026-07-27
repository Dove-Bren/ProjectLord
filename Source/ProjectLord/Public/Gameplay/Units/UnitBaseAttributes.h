// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "AttributeSet.h"

#include "UnitBaseAttributes.generated.h"

USTRUCT(BlueprintType)
struct PROJECTLORD_API FUnitBaseAttributes : public FTableRowBase
{
    GENERATED_BODY()

public:
    
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attributes")
    FGameplayAttribute Attribute;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attributes", Meta=(ClampMin=0))
    double BaseValue;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Base Attributes", Meta = (ClampMin = 0))
    double Variation;

};
