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
    void SetLevel(int InLevel) { UE_MVVM_SET_PROPERTY_VALUE(Level, InLevel); }

    float GetProgress() const { return Progress; }
    void SetProgress(float InProgress) { UE_MVVM_SET_PROPERTY_VALUE(Progress, InProgress); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Level")
    int Level;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Level")
    float Progress;
};
