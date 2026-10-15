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
class UGEHeroLevel;

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

DECLARE_MULTICAST_DELEGATE(FOnHeroXPChange);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHeroLevelUp, int /*NewLevel*/);

UCLASS(Blueprintable, meta = (PrioritizeCategories = "Hero Inventory"))
class PROJECTLORD_API AHeroBase : public ACreature
{
    GENERATED_BODY()

public:
    AHeroBase();

    FOnHeroXPChange OnHeroXPChange;
    FOnHeroLevelUp OnHeroLevelUp;

    UFUNCTION(BlueprintPure)
    int GetHeroLevel() const;

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

    UFUNCTION(BlueprintCallable, Category = "Hero")
    bool AttemptUseHealthPotion();

    UFUNCTION(BlueprintCallable, Category = "Hero")
    bool AttemptUseManaPotion();

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool HeroShouldFleeFromNearby() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetCombinedNearbyEnemyThreat() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetHeroCombinedConfidence() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetHeroSoloConfidence() const;

    virtual bool ShouldFlee() const override;

    virtual void Server_HandleUnitRecruited() override;

    virtual void BeginPlay() override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    virtual void NotifyResidenceAttacked(AResidentialBuilding* Building, AActor* AttackingActor, UCombatComponent* AttackingCombatComponent) override;

protected:

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
    TObjectPtr<ULordHeroAttributeSet> LordHeroAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combat")
    TSubclassOf<UGEHeroLevel> HeroLevelGameplayEffect;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UHeroEquipmentDef> StarterWeapon;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inventory")
    TObjectPtr<UHeroEquipmentDef> StarterArmor;

    UPROPERTY(EditDefaultsOnly, Category = "Hero")
    EEquipmentArchtype EquipmentType;

    // Bonus added when calculating flag scores.
    // Unit is roughly 100-gold's worth of motiviation.
    UPROPERTY(EditDefaultsOnly, Category = "Hero")
    TMap<ERewardFlagType, int> RewardFlagBonus;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero")
    TArray<FString> FirstNames;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hero")
    TArray<FString> LastNames;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Inventory")
    TObjectPtr<UHeroInventory> Inventory;

    // How much XP this hero has accumulated so far this level
    UPROPERTY(EditInstanceOnly, Category = "Hero", ReplicatedUsing = OnRep_HeroXP, Meta = (ClampMin = 0))
    int HeroXP;

    UPROPERTY(EditInstanceOnly, ReplicatedUsing = OnRep_HeroName, Category = "Hero")
    FText HeroName;

    virtual void Sided_RegisterAttributes() override;
    virtual void Server_SetupBaseAttributes() override;
    virtual void Client_SetupSelectionData(USelectionComponent* SelectionComponent) override;
    virtual bool ShouldHaveGravestone() const { return true; }
    virtual void Client_InitUnitVM() override;
    virtual UVMUnit* Client_ConstructUnitVM() override;
    virtual void OnDeath_Implementation() override;
    virtual void OnFinalDeath() override;

    virtual void HandleInventoryChange();

    void AddHeroXP(int Amount);

    void DoLevelUp();

    UFUNCTION(NetMulticast, Unreliable)
    void OnLevelUp();

    UFUNCTION(BlueprintImplementableEvent, Category = "Hero", meta = (DisplayName = "OnLevelUp"))
    void BP_OnLevelUp();

    UFUNCTION(NetMulticast, Unreliable)
    void OnGoldAwarded(int Amount);

    UFUNCTION(BlueprintImplementableEvent, Category = "Hero", meta = (DisplayName = "OnGoldAwarded"))
    void BP_OnGoldAwarded(int Amount);

    UFUNCTION(NetMulticast, Unreliable)
    void OnHealthPotionUsed();

    UFUNCTION(BlueprintImplementableEvent, Category = "Hero", meta = (DisplayName = "OnHealthPotionUsed"))
    void BP_OnHealthPotionUsed();

    UFUNCTION(NetMulticast, Unreliable)
    void OnManaPotionUsed();

    UFUNCTION(BlueprintImplementableEvent, Category = "Hero", meta = (DisplayName = "OnManaPotionUsed"))
    void BP_OnManaPotionUsed();

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Hero")
    bool ShouldUseHealthPotion() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Hero")
    bool ShouldUseManaPotion() const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Hero")
    FText GenerateHeroName() const;

    UPROPERTY()
    FActiveGameplayEffectHandle StrengthDamageModHandle;

    UPROPERTY()
    FActiveGameplayEffectHandle AgilityDamageModHandle;

    UPROPERTY()
    FActiveGameplayEffectHandle IntelligenceDamageModHandle;

    UPROPERTY()
    FActiveGameplayEffectHandle ConfidenceLevelModHandle;

    UPROPERTY()
    FActiveGameplayEffectHandle LevelModHandle;

    UPROPERTY(Replicated)
    TWeakObjectPtr<ARewardFlag> CurrentFlagTarget;

    void UpdateConfidenceLevelMod();

private:
    void UnapplyInventoryAttributes();
    void ApplyInventoryAttributes();

    TArray<const UHeroItemDef*> LastAppliedInventoryDefs;
    TArray<FActiveGameplayEffectHandle> LastAppliedInventoryEffects;

    UFUNCTION()
    void OnAttack(AActor* TargetActor, UCombatComponent* TargetCombatComponent);

    UFUNCTION()
    void OnHealthChanged(int Health, int MaxHealth);

    UFUNCTION()
    void OnRep_HeroXP();

    UFUNCTION()
    void OnRep_HeroName();

};
