// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "InspectableViewModel.generated.h"

class UInspectWidget;

DECLARE_DELEGATE(FOnInspect);
DECLARE_DELEGATE_RetVal(UVMLordBase*, FGetInspectVM);

UCLASS(BlueprintType)
class PROJECTLORD_API UVMInspectable : public UVMLordBase
{
    GENERATED_BODY()

public:

    static UVMInspectable* Make(UObject* Outer, FOnInspect OnInspect)
    {
        UVMInspectable* VM = CreateLordVM<UVMInspectable>(Outer);
        VM->InspectAction = OnInspect;
        return VM;
    }

    static UVMInspectable* Make(UObject* Outer, TSubclassOf<UInspectWidget> InspectWidgetClass, FGetInspectVM VMGetter);
    
    UFUNCTION(BlueprintCallable)
    void Inspect();

protected:

    FOnInspect InspectAction;
};
