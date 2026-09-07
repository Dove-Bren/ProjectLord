// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"

#include "SelectionActionViewModel.generated.h"

class USelectionAction;
enum class ESelectionActionFailureReason : uint8;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMSelectionAction : public UVMLordBase
{
    GENERATED_BODY()

public:
    FText GetName() const { return Name; }
    void SetName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(Name, InName); }

    FText GetDescription() const { return Description; }
    void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    int GetGoldCost() const { return GoldCost; }
    void SetGoldCost(int InGoldCost) { UE_MVVM_SET_PROPERTY_VALUE(GoldCost, InGoldCost); }

    bool GetEnabled() const { return bEnabled; }
    void SetEnabled(bool InEnabled, ESelectionActionFailureReason InDisableReason) { UE_MVVM_SET_PROPERTY_VALUE(DisableReason, InDisableReason); UE_MVVM_SET_PROPERTY_VALUE(bEnabled, InEnabled); }

    bool IsHidden() const { return bHidden; }
    void SetHidden(bool InHidden) { UE_MVVM_SET_PROPERTY_VALUE(bHidden, InHidden); }

    ESelectionActionFailureReason GetDisableReason() const { return DisableReason; }

    static UVMSelectionAction* Make(UObject* Outer, USelectionAction* Action);

    // Not part of normal use
    void OverrideAction(USelectionAction* NewAction);

    UFUNCTION(BlueprintCallable)
    void Perform();

    UFUNCTION(BlueprintCallable)
    void OnShown();


protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    FText Name;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    FText Description;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    ESelectionActionFailureReason DisableReason;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    int GoldCost;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = "GetEnabled", Category = "Selection|Action")
    bool bEnabled;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = "IsHidden", Category = "Selection|Action")
    bool bHidden;

    UPROPERTY()
    TWeakObjectPtr<USelectionAction> Action;
};
