// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"
#include "HeroInventoryViewModel.generated.h"

class UVMItemStack;
class UVMGold;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMHeroInventory : public UVMLordBase
{
    GENERATED_BODY()

public:

    UVMItemStack* GetWeapon() const { return Weapon; }
    void SetWeapon(UVMItemStack* InStack) { UE_MVVM_SET_PROPERTY_VALUE(Weapon, InStack); }

    UVMItemStack* GetArmor() const { return Armor; }
    void SetArmor(UVMItemStack* InStack) { UE_MVVM_SET_PROPERTY_VALUE(Armor, InStack); }

    UVMItemStack* GetHealthPotion() const { return HealthPotion; }
    void SetHealthPotion(UVMItemStack* InStack) { UE_MVVM_SET_PROPERTY_VALUE(HealthPotion, InStack); }

    UVMItemStack* GetManaPotion() const { return ManaPotion; }
    void SetManaPotion(UVMItemStack* InStack) { UE_MVVM_SET_PROPERTY_VALUE(ManaPotion, InStack); }

    TArray<UVMItemStack*> GetExtraItems() const { return ExtraItems; }
    void SetExtraItems(TArray<UVMItemStack*> InItems) { UE_MVVM_SET_PROPERTY_VALUE(ExtraItems, InItems); }

    UVMGold* GetGoldVM() const { return GoldVM; }
    void SetGoldVM(UVMGold* VM) { UE_MVVM_SET_PROPERTY_VALUE(GoldVM, VM); }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Inventory")
    TObjectPtr<UVMItemStack> Weapon;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Inventory")
    TObjectPtr<UVMItemStack> Armor;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Inventory")
    TObjectPtr<UVMItemStack> HealthPotion;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Inventory")
    TObjectPtr<UVMItemStack> ManaPotion;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Inventory")
    TArray<UVMItemStack*> ExtraItems;

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Inventory")
    TObjectPtr<UVMGold> GoldVM;

};
