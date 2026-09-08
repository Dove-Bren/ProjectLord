// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "ItemStackViewModel.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMItemStack : public UVMLordBase
{
    GENERATED_BODY()

public:

    FText GetItemName() const { return ItemName; }
    void SetItemName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(ItemName, InName); }

    FText GetDescription() const { return Description; }
    void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    int GetCount() const { return Count; }
    void SetCount(int InCount) { UE_MVVM_SET_PROPERTY_VALUE(Count, InCount); }

    bool DoesStack() const { return bStacks; }
    void SetStacks(bool bInStacks) { UE_MVVM_SET_PROPERTY_VALUE(bStacks, bInStacks); }
    

protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Item|Definition")
    FText ItemName;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Item|Definition")
    FText Description;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Item|Definition")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Item|Definition")
    int Count;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = DoesStack, Category = "Item|Definition")
    bool bStacks;
};
