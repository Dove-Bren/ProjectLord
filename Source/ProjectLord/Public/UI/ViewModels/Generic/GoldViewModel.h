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
    void SetGold(int InGold) { UE_MVVM_SET_PROPERTY_VALUE(Gold, InGold); UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetTotalGold); }

    int GetTaxGold() const { return TaxGold; }
    void SetTaxGold(int InTaxGold) { UE_MVVM_SET_PROPERTY_VALUE(TaxGold, InTaxGold); UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetTotalGold); }

    int GetGoldGeneration() const { return GoldGeneration; }
    void SetGoldGeneration(int InGoldGeneration) { UE_MVVM_SET_PROPERTY_VALUE(GoldGeneration, InGoldGeneration); }

    UFUNCTION(BlueprintPure, FieldNotify, Category = "Gold")
    int GetTotalGold() const { return Gold + TaxGold; }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Gold")
    int Gold;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Gold")
    int TaxGold;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Gold")
    int GoldGeneration;
};
