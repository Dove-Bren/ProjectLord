// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Creature.h"
#include "Gameplay/Units/HeroEquipment.h"

#include "HeroBase.generated.h"

class ULordHeroAttributeSet;
class UHeroInventory;

UCLASS(Blueprintable)
class PROJECTLORD_API AHeroBase : public ACreature
{
    GENERATED_BODY()

public:
    AHeroBase();

    UFUNCTION(BlueprintPure)
    int GetHeroXP() const { return HeroXP; }

    UFUNCTION(BlueprintPure)
    int GetHeroMaxXP() const;

    // Return how close to a hero level-up this hero is,
    // represented as a number between 0 (no progress) to 1 (enough xp to level up)
    UFUNCTION(BlueprintPure)
    float GetHeroXPPercent() const { return FMath::Clamp((float)GetHeroXP() / (float)GetHeroMaxXP(), 0.0f, 1.0f); }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    UHeroInventory* GetInventory() { return Inventory; }

    virtual void BeginPlay() override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<ULordHeroAttributeSet> LordHeroAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    FHeroEquipmentMap EquipmentTypes;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UHeroInventory> Inventory;

    // How much XP this hero has accumulated so far this level
    UPROPERTY(EditDefaultsOnly, Category = "Stats", Meta = (ClampMin = 0))
    int HeroXP;

    virtual void SetupBaseAttributes() override;

    virtual void HandleInventoryChange();

    void AddHeroXP(int Amount);

    UFUNCTION(BlueprintNativeEvent, Category = "Hero")
    void DoLevelUp();

private:
    void UnapplyInventoryAttributes();
    void ApplyInventoryAttributes();

    TArray<UHeroItemDef*> LastAppliedInventoryDefs;

    UFUNCTION()
    void OnAttack(AActor* TargetActor, UCombatComponent* TargetCombatComponent);

};
