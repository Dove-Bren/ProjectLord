// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "VisitorsViewModel.generated.h"

class UVMUnit;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMVisitors : public UVMLordBase
{
    GENERATED_BODY()

public:

    const TArray<UVMUnit*>& GetVisitors() const { return Visitors; }
    void SetVisitors(TArray<UVMUnit*> InVisitors) { UE_MVVM_SET_PROPERTY_VALUE(Visitors, InVisitors); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Visitors")
    TArray<UVMUnit*> Visitors;
};
