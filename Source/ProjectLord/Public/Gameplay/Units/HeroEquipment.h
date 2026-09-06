// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffect.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"

#include "HeroEquipment.generated.h"

class UTexture2D;

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

UENUM(BlueprintType)
enum class EEquipmentArchtype : uint8
{
    Melee,
    Ranged,
    Magic,
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
    TArray<TSubclassOf<UGameplayEffect>> GetItemEffects() const { return Effects; }

protected:
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    FText ItemName;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    FText ItemDescription;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    TObjectPtr<UTexture2D> ItemIcon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    EItemType ItemType = EItemType::Other;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    TArray<TSubclassOf<UGameplayEffect>> Effects;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    bool bCanStack;
};

UCLASS(Blueprintable)
class PROJECTLORD_API UHeroEquipmentDef : public UHeroItemDef
{
    GENERATED_BODY()

public:

    UFUNCTION(BlueprintPure, Category = "Item|Definintion")
    EEquipmentTier GetEquipmentTier() const { return Tier; }
    
    UFUNCTION(BlueprintPure, Category = "Item|Definintion")
    EEquipmentArchtype GetEquipmentArchtype() const { return Archtype; }

protected:

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    EEquipmentTier Tier;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    EEquipmentArchtype Archtype;
};

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroItemStack : public UObject
{
    GENERATED_BODY()

public:

    void Init(const UHeroItemDef* InItemDef, int InCount);
    static UHeroItemStack* Make(UObject* Outer, const UHeroItemDef* InItemDef, int InCount)
    {
        UHeroItemStack* Stack = NewObject<UHeroItemStack>(Outer);
        Stack->Init(InItemDef, InCount);
        return Stack;
    }

    UFUNCTION(BlueprintPure, Category = "ItemStack")
    const UHeroItemDef* GetItemDef() const { return ItemDef; }

    UFUNCTION(BlueprintPure, Category = "ItemStack")
    int GetCount() const { return Count; }

    UFUNCTION(BlueprintPure, Category = "ItemStack")
    int IsEmpty() const { return GetCount() == 0; }

    UFUNCTION(BlueprintCallable, Category = "ItemStack")
    int AddCount(int Amount) { return SetCount(GetCount() + Amount); }

    UFUNCTION(BlueprintCallable, Category = "ItemStack")
    int SetCount(int NewCount) { Count = FMath::Max(0, NewCount); return GetCount(); }

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemStack")
    TObjectPtr<const UHeroItemDef> ItemDef;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemStack")
    int Count;
};

DECLARE_MULTICAST_DELEGATE(FOnInventoryItemsChanged);
DECLARE_MULTICAST_DELEGATE(FOnInventoryGoldChanged);

UCLASS(BlueprintType)
class PROJECTLORD_API UHeroInventory : public UObject
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
