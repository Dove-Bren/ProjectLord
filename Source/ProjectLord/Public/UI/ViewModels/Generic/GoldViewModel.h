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
    int GetGoldGeneration() const { return GoldGeneration; }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Gold")
    int Gold;
    void SetGold(int InGold) { UE_MVVM_SET_PROPERTY_VALUE(Gold, InGold); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Gold")
    int GoldGeneration;
    void SetGoldGeneration(int InGoldGeneration) { UE_MVVM_SET_PROPERTY_VALUE(GoldGeneration, InGoldGeneration); }
};
