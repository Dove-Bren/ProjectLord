// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "CombatAbilityViewModel.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMCombatAbility : public UVMLordBase
{
    GENERATED_BODY()

public:

    FText GetAbilityName() const { return AbilityName; }
    void SetAbilityName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(AbilityName, InName); }

    FText GetDescription() const { return Description; }
    void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }


protected:
    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Ability|Definition")
    FText AbilityName;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Ability|Definition")
    FText Description;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Ability|Definition")
    TObjectPtr<UTexture2D> Icon;
};
