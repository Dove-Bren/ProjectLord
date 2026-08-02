// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "UI/ViewModels/LordViewModelBase.h"

#include "SummarySlotsViewModel.generated.h"

class UVMSummarySlot;
class UTexture2D;
class UHeroItemStack;
class UUnitType;

// Meant to represent the 6 slots shown in the bottom right of the selection UI.
// What's shown varies wildly depending on the type of thing selected.
// This VM wraps that up and provides very general sub VMs  to show there.
UCLASS(BlueprintType)
class PROJECTLORD_API UVMSummarySlots : public UVMLordBase
{
    GENERATED_BODY()

public:
    UVMSummarySlots();

    void Init();

    const TArray<UVMSummarySlot*> GetSlots() const { return Slots; }
    void SetSlot(int Index, UVMSummarySlot* InSlot)
    {
        if (Index >= 0 && Index < 6
            && Slots[Index] != InSlot)
        {
            Slots[Index] = InSlot;
            UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(Slots);
        }
    }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Slots")
    TArray<UVMSummarySlot*> Slots;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UVMSummarySlot : public UVMLordBase
{
    GENERATED_BODY()

public:

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    int GetCount() const { return Count; }
    void SetCount(int InCount) { UE_MVVM_SET_PROPERTY_VALUE(Count, InCount); }

    int GetMaxCount() const { return MaxCount; }
    void SetMaxCount(int InMaxCount) { UE_MVVM_SET_PROPERTY_VALUE(MaxCount, InMaxCount); }

    static UVMSummarySlot* MakeEmpty(UObject* Outer) { return CreateLordVM<UVMSummarySlot>(Outer); }
    static UVMSummarySlot* MakeItem(UObject* Outer, const UHeroItemStack* Stack);
    static UVMSummarySlot* MakeUnitTypeCount(UObject* Outer, UUnitType* Type);
    static UVMSummarySlot* MakeVisitorCount(UObject* Outer);

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Slots")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Slots")
    int Count;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Slots")
    int MaxCount;

};
