// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameplayEffect.h"
#include "Gameplay/Attributes/AttributeBaseValue.h"
#include "UI/ViewModels/Generic/StatModifiersViewModel.h"

#include "HeroEquipment.generated.h"

class UTexture2D;
class UVMHeroInventory;
class UVMItemStack;

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

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    TSubclassOf<UCombatAbility> GetUseAbility() const { return UseAbility; }

    UFUNCTION(BlueprintPure, Category = "Item|Definition")
    const FStatModifiers& GetPreviewModifiers() const { return PreviewModifiers; }

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

    // Ability to use on a unit when it uses this item
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    TSubclassOf<UCombatAbility> UseAbility;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Definition")
    FStatModifiers PreviewModifiers;

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
    int DecrementCount() { return AddCount(-1); }

    UFUNCTION(BlueprintCallable, Category = "ItemStack")
    int SetCount(int NewCount);

    UFUNCTION(BlueprintCallable, Category = "ItemStack")
    UVMItemStack* MakeViewModel();

    virtual bool IsSupportedForNetworking() const override { return true; }
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_Def, Category = "ItemStack")
    TObjectPtr<const UHeroItemDef> ItemDef;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, ReplicatedUsing = OnRep_Count, Category = "ItemStack")
    int Count;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "ItemStack")
    TObjectPtr<UVMItemStack> ViewModel;

private:
    UFUNCTION()
    void OnRep_Def();

    UFUNCTION()
    void OnRep_Count();
};
