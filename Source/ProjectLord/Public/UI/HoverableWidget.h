// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HoverableWidget.generated.h"

struct FStaticSelection;

UCLASS(Blueprintable, Abstract)
class PROJECTLORD_API UHoverableWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure)
    virtual bool CanHover() const { return true; }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
    FStaticSelection GetHoverInfo();



protected:

    UFUNCTION(BlueprintCallable)
    void PushHoverInfoToController();

    UFUNCTION(BlueprintCallable)
    void ClearHoverInfoFromController();

    virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
    virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

};
