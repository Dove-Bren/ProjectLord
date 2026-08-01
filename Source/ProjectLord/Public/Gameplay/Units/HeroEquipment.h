// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HeroEquipment.generated.h"

class UTexture2D;
struct FAttributeBaseValue;
class UCreatureType;

UENUM(BlueprintType)
enum class EItemType : uint8
{
    Weapon,
    Armor,
    HealthPotion,
    ManaPotion,
    Other
};

UENUM(BlueprintType)
enum class EEquipmentTier : uint8
{
    Starter,
    SecondTier,
    ThirdTier,
    FourthTier,

    Invalid
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

    UFUNCTION(BlueprintPure, Category = "Item|Definintion")
    EItemType GetItemType() const { return ItemType; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    bool GetCanStack() const { return bCanStack; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    TArray<FAttributeBaseValue> GetAttributesToApply() const { return AttributesToApply; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    virtual bool CanUse(UCreatureType* HeroType) const { return true; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    FText ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    FText ItemDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    TObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    EItemType ItemType = EItemType::Other;

    // TODO replace with GameplayEffect, which can apply attribute modifiers too
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    TArray<FAttributeBaseValue> AttributesToApply;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    bool bCanStack;
};

UCLASS(Blueprintable)
class PROJECTLORD_API UHeroEquipmentDef : public UHeroItemDef
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Item|Definintion")
    EEquipmentTier GetEquipmentTier() const { return EEquipmentTier::Starter; }

    virtual bool CanUse(UCreatureType* HeroType) const override;

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    EEquipmentTier Tier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    TArray<UCreatureType*> AllowedHeroTypes;

};

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroItemStack : public UObject
{
    GENERATED_BODY()

public:

    void Init(UHeroItemDef* InItemDef, int InCount);
    static UHeroItemStack* Make(UObject* Outer, UHeroItemDef* InItemDef, int InCount)
    {
        UHeroItemStack* Stack = NewObject<UHeroItemStack>(Outer);
        Stack->Init(InItemDef, InCount);
        return Stack;
    }

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

USTRUCT(BlueprintType)
struct PROJECTLORD_API FHeroEquipmentMap
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

DECLARE_MULTICAST_DELEGATE(FOnInventoryItemsChanged);

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroInventory : public UObject
{
    GENERATED_BODY()

public:

    UHeroInventory();

    void InitInventory(UHeroEquipmentDef* StarterWeapon, UHeroEquipmentDef* StarterArmor);

    FOnInventoryItemsChanged OnInventoryItemsChanged;

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

    /*bool ItemIsWeapon(const UHeroItemStack* Item) const;
    EEquipmentTier GetWeaponTier(const UHeroItemStack* Item) const;
    bool ItemIsArmor(const UHeroItemStack* Item) const;
    EEquipmentTier GetArmorTier(const UHeroItemStack* Item) const;
    bool ItemIsHealthPotion(const UHeroItemStack* Item) const;
    bool ItemIsManaPotion(const UHeroItemStack* Item) const;*/
    bool AddExtraItem(UHeroItemStack* ExtraItem);

private:

    static const int MaxExtraSlots = 8;

    static UHeroItemStack* MakeThrowawayStack(UObject* Outer, UHeroItemDef* Def, int Count);
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
