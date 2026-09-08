// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InspectWidget.generated.h"

class UVMLordBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnClose);

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UInspectWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    
    // Attempt to setup with the provided VM.
    // Implementations are expected to have to cast the VM, and should
    // return failure (false) if the VM is not the correct type.
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    bool Setup(UVMLordBase* VM);

    UPROPERTY(BlueprintAssignable, BlueprintCallable)
    FOnClose OnClose;

};
