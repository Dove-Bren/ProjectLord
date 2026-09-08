// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "BuildingGoodsViewModel.generated.h"

class UVMGameGood;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMBuildingGoods : public UVMLordBase
{
    GENERATED_BODY()

public:

    const TArray<UVMGameGood*>& GetGoods() const { return Goods; }
    void SetGoods(TArray<UVMGameGood*> InGoods) { UE_MVVM_SET_PROPERTY_VALUE(Goods, InGoods); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "BuildingGoods")
    TArray<UVMGameGood*> Goods;

};
