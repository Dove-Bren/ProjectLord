// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "LevelViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMLevel : public UVMLordBase
{
    GENERATED_BODY()

public:
    int GetLevel() const { return Level; }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Level")
    int Level;
    void SetLevel(int InLevel) { UE_MVVM_SET_PROPERTY_VALUE(Level, InLevel); }
};
