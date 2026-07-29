// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HeroEquipment.generated.h"

class UTexture2D;

UENUM(BlueprintType)
enum class EEquipmentTier : uint8
{
    Starter,
    SecondTier,
    ThirdTier,
    FourthTier,
};

UCLASS(Blueprintable)
class PROJECTLORD_API UHeroItemDef : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    FText GetItemName() const { return ItemName; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    FText GetItemDescription() const { return ItemDescription; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    UTexture2D* GetItemIcon() const { return ItemIcon; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    bool GetCanStack() const { return bCanStack; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    UDataTable* GetAttributesToApply() const { return AttributesToApply; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    FText ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    FText ItemDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    TObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.UnitBaseAttributes"))
    TObjectPtr<UDataTable> AttributesToApply;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    bool bCanStack;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroItemStack : public UObject
{
    GENERATED_BODY()

public:

    void Init(UHeroItemDef* InItemDef, int InCount);

    UFUNCTION(BlueprintPure, Category = "ItemStack")
    UHeroItemDef* GetItemDef() const { return ItemDef; }

    UFUNCTION(BlueprintPure, Category = "ItemStack")
    int GetCount() const { return Count; }

    UFUNCTION(BlueprintPure, Category = "ItemStack")
    int IsEmpty() const { return GetCount() == 0; }

    UFUNCTION(BlueprintCallable, Category = "ItemStack")
    int AddCount(int Amount) { return SetCount(GetCount() + Amount); }

    UFUNCTION(BlueprintCallable, Category = "ItemStack")
    int SetCount(int NewCount) { Count = FMath::Max(0, GetCount() + NewCount); return GetCount(); }

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemStack")
    TObjectPtr<UHeroItemDef> ItemDef;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemStack")
    int Count;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroEquipmentMap : public UObject
{
    GENERATED_BODY()

public:
    // Could map tier to def, but this way there has to be a mapping for each and it reads better in the editor

    // What weapon is the Starter weapon for this hero type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Weapon")
    TObjectPtr<UHeroItemDef> Weapon_Starter;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Weapon")
    TObjectPtr<UHeroItemDef> Weapon_SecondTier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Weapon")
    TObjectPtr<UHeroItemDef> Weapon_ThirdTier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Weapon")
    TObjectPtr<UHeroItemDef> Weapon_FourthTier;

    // What weapon is the Starter weapon for this hero type
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Armor")
    TObjectPtr<UHeroItemDef> Armor_Starter;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Armor")
    TObjectPtr<UHeroItemDef> Armor_SecondTier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Armor")
    TObjectPtr<UHeroItemDef> Armor_ThirdTier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Armor")
    TObjectPtr<UHeroItemDef> Armor_FourthTier;

    // I think these will be standard but don't want to invent a new mechanism
    // for getting them...

    // Health potion item def
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Potion")
    TObjectPtr<UHeroItemDef> HealthPotion;

    // Health potion item def
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "EquipmentMap|Equipment|Potion")
    TObjectPtr<UHeroItemDef> ManaPotion;

    UHeroItemDef* GetWeaponForTier(EEquipmentTier Tier) const
    {
        switch (Tier)
        {
        case EEquipmentTier::Starter:
        default:
            return Weapon_Starter;
        case EEquipmentTier::SecondTier:
            return Weapon_SecondTier;
        case EEquipmentTier::ThirdTier:
            return Weapon_ThirdTier;
        case EEquipmentTier::FourthTier:
            return Weapon_FourthTier;
        }
    }

    UHeroItemDef* GetArmorForTier(EEquipmentTier Tier) const
    {
        switch (Tier)
        {
        case EEquipmentTier::Starter:
        default:
            return Armor_Starter;
        case EEquipmentTier::SecondTier:
            return Armor_SecondTier;
        case EEquipmentTier::ThirdTier:
            return Armor_ThirdTier;
        case EEquipmentTier::FourthTier:
            return Armor_FourthTier;
        }
    }

};

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroInventory : public UObject
{
    GENERATED_BODY()

public:

    UHeroInventory();

    void InitInventory(const UHeroEquipmentMap* EquipmentDefMap);

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    EEquipmentTier GetWeaponTier() const { return Weapon; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    EEquipmentTier GetArmorTier() const { return Armor; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    int GetNumHealthPotions() const { return HealthPotions; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    int GetNumManaPotions() const { return ManaPotions; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Gold")
    int GetPersonalGold() const { return PersonalGold; }

    UFUNCTION(BlueprintPure, Category = "Inventory|Gold")
    int GetGuildGold() const { return GuildGold; }

    UFUNCTION(BlueprintPure, Category = "Inventory|ExtraItems")
    TArray<UHeroItemStack*> GetExtraSlots() const { return  ExtraSlots; }

    // ItemStack interface for Equipment

    // Get an ItemStack representing the weapon in this inventory.
    // Note the stack is constructed on the fly, and changes do not propogate
    // to the inventory.
    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetWeaponAsStack() const;

    // Get an ItemStack representing the armor in this inventory.
    // Note the stack is constructed on the fly, and changes do not propogate
    // to the inventory.
    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetArmorAsStack() const;

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetHealthPotionsAsStack() const;

    UFUNCTION(BlueprintPure, Category = "Inventory|Equipment")
    UHeroItemStack* GetManaPotionsAsStack() const;

    // More general queries

    // Check for the provided item def **in the Extra Item slots**.
    // This does not match armor, weapons, or health/mana potions.
    UFUNCTION(BlueprintPure, Category = "Inventory")
    UHeroItemStack* FindItem(const UHeroItemDef* ItemDef) const;

    // Check for the provided item def **in the Extra Item slots**.
    // This does not match armor, weapons, or health/mana potions.
    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool HasItem(const UHeroItemDef* ItemDef) const { return IsValid(FindItem(ItemDef)); }

    // Modifications

    UFUNCTION(BlueprintCallable, Category = "Inventory|ExtraItems")
    bool AddExtraItem(UHeroItemStack* ExtraItem);

    

protected:

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    EEquipmentTier Weapon;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    EEquipmentTier Armor;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    int HealthPotions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Equipment")
    int ManaPotions;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Gold")
    int PersonalGold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|Gold")
    int GuildGold;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory|ExtraItems")
    TArray<UHeroItemStack*> ExtraSlots;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Inventory|Definition")
    TObjectPtr<const UHeroEquipmentMap> EquipmentDefMap;

private:

    static const int MaxExtraSlots = 8;

    static UHeroItemStack* MakeThrowawayStack(UObject* Outer, UHeroItemDef* Def, int Count);
};
