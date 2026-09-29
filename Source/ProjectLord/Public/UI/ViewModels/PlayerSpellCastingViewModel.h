// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "PlayerSpellCastingViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMPlayerSpellCasting : public UVMLordBase
{
    GENERATED_BODY()

public:
    
    int GetCurrentCost() const { return CurrentCost; }
    void SetCurrentCost(int InCost) { UE_MVVM_SET_PROPERTY_VALUE(CurrentCost, InCost); }

    bool CanAfford() const { return bCanAfford; }
    void SetCanAfford(bool bInCanAfford) { UE_MVVM_SET_PROPERTY_VALUE(bCanAfford, bInCanAfford); }

    bool CanCast() const { return bCanCast; }
    void SetCanCast(bool bInCanCast) { UE_MVVM_SET_PROPERTY_VALUE(bCanCast, bInCanCast); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "SpellCasting")
    int CurrentCost;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = CanAfford, Category = "SpellCasting")
    bool bCanAfford;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = CanCast, Category = "SpellCasting")
    bool bCanCast;
};
