// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "ActionViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMAction : public UVMLordBase
{
    GENERATED_BODY()

public:
    FText GetAction() const { return Action; }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Action")
    FText Action;
    void SetAction(FText InAction) { UE_MVVM_SET_PROPERTY_VALUE(Action, InAction); }
};
