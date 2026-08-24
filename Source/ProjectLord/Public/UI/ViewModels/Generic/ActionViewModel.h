// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "Gameplay/Units/CreatureAction.h"

#include "ActionViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMAction : public UVMLordBase
{
    GENERATED_BODY()

public:
    ECreatureAction GetAction() const { return Action; }
    void SetAction(ECreatureAction InAction) { UE_MVVM_SET_PROPERTY_VALUE(Action, InAction); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Action")
    ECreatureAction Action;
};
