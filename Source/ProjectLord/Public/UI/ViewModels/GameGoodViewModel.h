// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "GameGoodViewModel.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMGameGood : public UVMLordBase
{
    GENERATED_BODY()

public:

    FText GetName() const { return Name; }
    void SetName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(Name, InName); }

    FText GetDescription() const { return Description; }
    void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Good")
    FText Name;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Good")
    FText Description;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Good")
    TObjectPtr<UTexture2D> Icon;

};
