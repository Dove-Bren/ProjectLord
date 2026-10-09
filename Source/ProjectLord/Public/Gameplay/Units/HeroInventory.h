// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Gameplay/Units/HeroEquipment.h"

#include "HeroInventory.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryItemsChanged);
DECLARE_MULTICAST_DELEGATE(FOnInventoryGoldChanged);

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroInventory : public UActorComponent
{
    GENERATED_BODY()

public:

    UHeroInventory();

    void InitInventory(UHeroEquipmentDef* StarterWeapon, UHeroEquipmentDef* StarterArmor);

    FOnInventoryItemsChanged OnInventoryItemsChanged;
    FOnInventoryGoldChanged OnInventoryGoldChanged;

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetWeapon() const { return Weapon; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetArmor() const { return Armor; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    EEquipmentTier GetWeaponTier() const { return UnwrapEquipmentTier(Weapon); }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    EEquipmentTier GetArmorTier() const { return UnwrapEquipmentTier(Armor); }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetHealthPotions() const { return HealthPotions; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetManaPotions() const { return ManaPotions; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    int GetNumHealthPotions() const { return HealthPotions ? HealthPotions->GetCount() : 0; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    int GetNumManaPotions() const { return ManaPotions ? ManaPotions->GetCount() : 0; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Gold")
    int GetPersonalGold() const { return PersonalGold; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Gold")
    int GetGuildGold() const { return GuildGold; }

    UFUNCTION(BlueprintPure, Category = "Inventory|ExtraItems")
    TArray<UHeroItemStack*> GetExtraSlots() const { return  ExtraSlots; }

    // More general queries

    // Check for the provided item def **in the Weapon, Armor, and Extra Item slots**.
    // This does not match health/mana potions.
    UFUNCTION(BlueprintPure, Category = "Inventory")
    UHeroItemStack* FindItem(const UHeroItemDef* ItemDef) const;

    // Check for the provided item def **in the Weapon, Armor, and Extra Item slots**.
    // This does not match health/mana potions.
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasItem(const UHeroItemDef* ItemDef) const { return IsValid(FindItem(ItemDef)); }

    // Modifications

    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    void SetWeapon(UHeroItemStack* Weapon);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    void SetArmor(UHeroItemStack* Armor);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    void SetHealthPotions(UHeroItemStack* HealthPotions);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    void DeductHealthPotion();

    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    void SetManaPotions(UHeroItemStack* ManaPotions);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Equipment")
    void DeductManaPotion();

    UFUNCTION(BlueprintCallable, Category = "Inventory|Gold")
    int AddPersonalGold(int Amount);

    UFUNCTION(BlueprintCallable, Category = "Inventory|Gold")
    int AddGuildGold(int Amount);

    UFUNCTION(BlueprintCallable, Category = "Inventory|ExtraItems")
    bool Add(UHeroItemStack* Item);

    UFUNCTION(BlueprintPure, Category = "Inventory|ExtraItems")
    bool CanFit(UHeroItemStack* Item);

    UFUNCTION(BlueprintPure, Category = "Inventory")
    UVMHeroInventory* GetViewModel() const { return ViewModel; }


protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    UHeroItemStack* Weapon;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    UHeroItemStack* Armor;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    UHeroItemStack* HealthPotions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    UHeroItemStack* ManaPotions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Gold")
    int PersonalGold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Gold")
    int GuildGold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|ExtraItems")
    TArray<UHeroItemStack*> ExtraSlots;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|ExtraItems")
    TObjectPtr<UVMHeroInventory> ViewModel;

    bool AddExtraItem(UHeroItemStack* ExtraItem, bool bSimulateOnly = false);

private:

    static const int MaxExtraSlots = 8;

    static EEquipmentTier UnwrapEquipmentTier(const UHeroItemStack* Stack)
    {
        const auto* EquipDef = Cast<UHeroEquipmentDef>(Stack->GetItemDef());
        if (ensure(EquipDef))
        {
            return EquipDef->GetEquipmentTier();
        }
        return EEquipmentTier::Starter;
    }
};
