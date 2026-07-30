// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Blueprint/UserWidget.h"

#include "HealthBarBase.generated.h"

class UVMUnit;

UCLASS()
class PROJECTLORD_API UHealthBarBase : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintImplementableEvent)
    void ReceiveUnitVM(UVMUnit* InViewModel);
};
