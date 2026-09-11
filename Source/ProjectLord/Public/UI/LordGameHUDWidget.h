// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LordGameHUDWidget.generated.h"

struct FToastNotification;
class UVMToast;

UCLASS(Blueprintable)
class PROJECTLORD_API ULordGameHUDWidget : public UUserWidget
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD|Toast")
    void AddToastNotification(FToastNotification Notification);


protected:

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "HUD|Toast")
    void PushToastNotification(UVMToast* Notification);

    
};
