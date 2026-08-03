// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"

#include "SelectionActionViewModel.generated.h"

class USelectionAction;

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
    void SetEnabled(bool InEnabled) { UE_MVVM_SET_PROPERTY_VALUE(bEnabled, InEnabled); }

    static UVMSelectionAction* Make(UObject* Outer, USelectionAction* Action);

    UFUNCTION(BlueprintCallable)
    void Perform();


protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    FText Name;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    FText Description;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Selection|Action")
    int GoldCost;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = "GetEnabled", Category = "Selection|Action")
    bool bEnabled;

    UPROPERTY()
    TWeakObjectPtr<USelectionAction> Action;
};
