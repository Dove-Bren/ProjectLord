// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "ResidentsViewModel.generated.h"

class UVMUnit;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMResidents : public UVMLordBase
{
    GENERATED_BODY()

public:

    const TArray<UVMUnit*>& GetResidents() const { return Residents; }
    void SetResidents(TArray<UVMUnit*> InResidents);

    const bool CanHaveResidents() const { return bCanHaveResidents; }
    void SetCanHaveResidents(bool bInCanHaveResidents) { UE_MVVM_SET_PROPERTY_VALUE(bCanHaveResidents, bInCanHaveResidents); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Residents")
    TArray<UVMUnit*> Residents;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = CanHaveResidents, Category = "Residents")
    bool bCanHaveResidents;

};
