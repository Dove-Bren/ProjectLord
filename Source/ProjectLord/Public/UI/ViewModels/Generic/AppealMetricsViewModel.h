// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"

#include "AppealMetricsViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMAppealMetrics : public UVMLordBase
{
    GENERATED_BODY()

public:
    
    int GetInterestedCount() const { return InterestedCount; }
    void SetInterestedCount(int InCount) { UE_MVVM_SET_PROPERTY_VALUE(InterestedCount, InCount); }

protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Interest")
    int InterestedCount;
};
