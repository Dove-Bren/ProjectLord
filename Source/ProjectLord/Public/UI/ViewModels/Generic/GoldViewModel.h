// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "GoldViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMGold : public UVMLordBase
{
    GENERATED_BODY()

public:
    
    int GetGold() const { return Gold; }
    void SetGold(int InGold) { UE_MVVM_SET_PROPERTY_VALUE(Gold, InGold); }

    int GetGoldGeneration() const { return GoldGeneration; }
    void SetGoldGeneration(int InGoldGeneration) { UE_MVVM_SET_PROPERTY_VALUE(GoldGeneration, InGoldGeneration); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Gold")
    int Gold;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Gold")
    int GoldGeneration;
};
