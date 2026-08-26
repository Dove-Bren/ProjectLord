// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Gameplay/Units/UnitTypes.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "CreatureCategoryViewModel.generated.h"

UCLASS(BlueprintType)
class PROJECTLORD_API UVMCreatureCategory : public UVMLordBase
{
    GENERATED_BODY()

public:

    static UVMCreatureCategory* Make(UObject* Outer, ECreatureCategory Category)
    {
        UVMCreatureCategory* VM = CreateLordVM<UVMCreatureCategory>(Outer);
        VM->SetCategory(Category);
        return VM;
    }

    ECreatureCategory GetCategory() const { return Category; }
    void SetCategory(ECreatureCategory InCategory) { UE_MVVM_SET_PROPERTY_VALUE(Category, InCategory); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Category")
    ECreatureCategory Category;
};
