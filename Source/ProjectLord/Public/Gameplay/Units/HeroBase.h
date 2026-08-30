// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "Gameplay/Units/Creature.h"
#include "Gameplay/Units/HeroEquipment.h"
#include "Gameplay/Units/RewardFlag.h"

#include "HeroBase.generated.h"

class ULordHeroAttributeSet;
class UHeroInventory;
class UGameGood;
class UGameplayAbility;

struct FActiveGameplayEffectHandle;

UENUM(BlueprintType)
enum class EHeroDesireLevel : uint8
{
    NotApplicable,
    StrongDislike,
    Dislike,
    Neutral,
    Like,
    StrongLike,
};

DECLARE_MULTICAST_DELEGATE(FOnXPChange);

UCLASS(Blueprintable, meta = (PrioritizeCategories = "Hero Inventory"))
class PROJECTLORD_API AHeroBase : public ACreature
{
    GENERATED_BODY()

public:
    AHeroBase();

    FOnXPChange OnXPChange;

    UFUNCTION(BlueprintPure)
    int GetHeroXP() const { return HeroXP; }

    UFUNCTION(BlueprintPure)
    int GetHeroMaxXP() const;

    UFUNCTION(BlueprintPure)
    FText GetHeroName() const { return HeroName; }

    // Return how close to a hero level-up this hero is,
    // represented as a number between 0 (no progress) to 1 (enough xp to level up)
    UFUNCTION(BlueprintPure)
    float GetHeroXPPercent() const { return FMath::Clamp((float)GetHeroXP() / (float)GetHeroMaxXP(), 0.0f, 1.0f); }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    UHeroInventory* GetInventory() { return Inventory; }

    UFUNCTION(BlueprintPure, Category = "Inventory")
    bool CanApply(const UGameGood* Good) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    void Apply(const UGameGood* Good);

    UFUNCTION(BlueprintCallable, Category = "Hero")
    void AwardGold(int Amount);

    UFUNCTION(BlueprintPure, Category = "Hero")
    EEquipmentArchtype GetHeroEquipmentType() const { return EquipmentType; }

    UFUNCTION(BlueprintPure, Category = "Hero|Desires")
    EHeroDesireLevel CheckDesireLevelForItem(const UHeroItemDef* Item) const;

    UFUNCTION(BlueprintPure, Category = "Hero|Desires")
    bool HasItemOrBetter(const UHeroItemDef* Item) const;

    // Whether this hero could accept this item type.
    // For example, is it the right type of equipment if it's
    // equipment, or is there room for it in the extra slots if it goes there?
    UFUNCTION(BlueprintPure, Category = "Hero|Desires")
    bool CanAcceptItem(const UHeroItemDef* Item) const;

    UFUNCTION(BlueprintPure, Category = "Hero|Desires")
    EHeroDesireLevel CheckDesireLevelForGood(const UGameGood* Good) const;

    UFUNCTION(BlueprintPure, Category = "Hero")
    bool HasAbility(TSubclassOf<UGameplayAbility> AbilityClass) const;

    UFUNCTION(BlueprintCallable, Category = "Inventory")
    bool PurchaseGood(FGoodOffer Offer);

    // Return a score that represents this heroes eagerness to persue this flag.
    // A score of 0 or less indicates no interest.
    UFUNCTION(BlueprintPure, Category = "Reward Flag")
    int ScoreFlag(ARewardFlag* Flag) const;

    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void SetFlagTarget(ARewardFlag* Flag);

    UFUNCTION(BlueprintCallable, Category = "Reward Flag")
    void ClearFlagTarget() { SetFlagTarget(nullptr); }

    virtual void BeginPlay() override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<ULordHeroAttributeSet> LordHeroAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    UHeroEquipmentDef* StarterWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    UHeroEquipmentDef* StarterArmor;

    UPROPERTY(EditDefaultsOnly, Category = "Hero")
    EEquipmentArchtype EquipmentType;

    // Bonus added when calculating flag scores.
    // Unit is roughly 100-gold's worth of motiviation.
    UPROPERTY(EditDefaultsOnly, Category = "Hero")
    TMap<ERewardFlagType, int> RewardFlagBonus;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "Inventory")
    TObjectPtr<UHeroInventory> Inventory;

    // How much XP this hero has accumulated so far this level
    UPROPERTY(EditInstanceOnly, Category = "Hero", Meta = (ClampMin = 0))
    int HeroXP;

    UPROPERTY(EditInstanceOnly, Category = "Hero")
    FText HeroName;

    virtual void SetupBaseAttributes() override;
    virtual void SetupSelectionData(USelectionComponent* SelectionComponent) override;
    virtual bool ShouldHaveGravestone() const { return true; }
    virtual void InitUnitVM() override;

    virtual void HandleInventoryChange();

    void AddHeroXP(int Amount);

    void DoLevelUp();

    UFUNCTION(BlueprintImplementableEvent, Category = "Hero")
    void OnLevelUp();

    UFUNCTION(BlueprintImplementableEvent, Category = "Hero")
    void OnGoldAwarded(int Amount);

    UPROPERTY()
    FActiveGameplayEffectHandle StrengthDamageModHandle;

    UPROPERTY()
    FActiveGameplayEffectHandle AgilityDamageModHandle;

    UPROPERTY()
    FActiveGameplayEffectHandle IntelligenceDamageModHandle;

    UPROPERTY()
    TWeakObjectPtr<ARewardFlag> CurrentFlagTarget;

    void UpdateAttributeDamageMod(FActiveGameplayEffectHandle& Handle, int Attribute);

    static float DamageModPerAttribute;

private:
    void UnapplyInventoryAttributes();
    void ApplyInventoryAttributes();

    TArray<const UHeroItemDef*> LastAppliedInventoryDefs;
    TArray<FActiveGameplayEffectHandle> LastAppliedInventoryEffects;

    UFUNCTION()
    void OnAttack(AActor* TargetActor, UCombatComponent* TargetCombatComponent);

};
