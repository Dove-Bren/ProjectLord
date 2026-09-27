// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "PlayerSpellViewModel.generated.h"

class UTexture2D;
class UPlayerSpell;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMPlayerSpell : public UVMLordBase
{
    GENERATED_BODY()

public:

    FText GetSpellName() const { return SpellName; }
    void SetSpellName(FText InName) { UE_MVVM_SET_PROPERTY_VALUE(SpellName, InName); }

    FText GetDescription() const { return Description; }
    void SetDescription(FText InDescription) { UE_MVVM_SET_PROPERTY_VALUE(Description, InDescription); }

    UTexture2D* GetIcon() const { return Icon; }
    void SetIcon(UTexture2D* InIcon) { UE_MVVM_SET_PROPERTY_VALUE(Icon, InIcon); }

    int GetBaseCost() const { return BaseCost; }
    void SetBaseCost(int InBaseCost) { UE_MVVM_SET_PROPERTY_VALUE(BaseCost, InBaseCost); }

    bool CanAfford() const { return bCanAfford; }
    void SetCanAfford(bool bInAfford) { UE_MVVM_SET_PROPERTY_VALUE(bCanAfford, bInAfford); }

    void SetSpellToCast(TWeakObjectPtr<UPlayerSpell> InSpell) { SpellToCast = InSpell; }

    UFUNCTION(BlueprintCallable, Category = "Spell", meta = (WorldContext = "WorldContextObject"))
    void StartCasting(const UObject* WorldContextObject);

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Spell|Definition")
    FText SpellName;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Spell|Definition")
    FText Description;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Spell|Definition")
    TObjectPtr<UTexture2D> Icon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Spell|Definition")
    int BaseCost;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter = CanAfford, Category = "Spell|Definition")
    bool bCanAfford;

    UPROPERTY()
    TWeakObjectPtr<UPlayerSpell> SpellToCast;
};
