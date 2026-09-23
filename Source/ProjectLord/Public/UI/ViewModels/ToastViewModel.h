// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "UI/ToastNotification.h"

#include "ToastViewModel.generated.h"

class UTexture2D;
class USelectionComponent;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMToast : public UVMLordBase
{
    GENERATED_BODY()

public:
    
    EToastNotificationType GetType() const { return Type; }
    void SetType(EToastNotificationType InType) { UE_MVVM_SET_PROPERTY_VALUE(Type, InType); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    FText GetContent() const { return Content; }
    void SetContent(FText InContent) { UE_MVVM_SET_PROPERTY_VALUE(Content, InContent); }

    void SetSelectSourceComponent(USelectionComponent* Source) { SelectSourceComponent = Source; }

    UFUNCTION(BlueprintCallable)
    void SelectSource();


protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Toast")
    EToastNotificationType Type;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Toast")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Toast")
    FText Content;

    TObjectPtr<USelectionComponent> SelectSourceComponent;
};
