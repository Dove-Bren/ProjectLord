// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"

#include "Gameplay/Combat/CombatTypes.h"
#include "Gameplay/Units/UnitTypes.h"
#include "Gameplay/Combat/Ability/UnitAbility.h" // Including instead of forward declared to make dropdowns populate right

#include "Unit.generated.h"

class AUnitController;
class UAbilitySystemComponent;
class ULordUnitAttributeSet;

struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

UCLASS(Blueprintable)
class PROJECTLORD_API AUnit : public ACharacter
{
    GENERATED_BODY()

public:
    AUnit();


    UFUNCTION(BlueprintPure)
    AUnitController* GetUnitController() const;

    UFUNCTION(BlueprintPure)
    bool IsDead() const;

    UFUNCTION(BlueprintPure)
    bool IsAlive() const { return !IsDead(); }

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsCloseEnoughToAttack(const AUnit* OtherUnit) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Combat|State")
    bool CanAttack() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetDefenseFor(EDamageType InType) const;
    
    // TODO: Remove and make callers use attribute system?
    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetMeleeDefense() const { return GetDefenseFor(EDamageType::Melee); }

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetRangedDefense() const { return GetDefenseFor(EDamageType::Ranged); }

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetMagicDefense() const { return GetDefenseFor(EDamageType::Magic); }

    // Attack the passed in unit as this unit
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void AttackUnit(AUnit* TargetUnit);

    // Get all Unit Abilities this Unit has access to
    UFUNCTION(BlueprintPure, Category="Ability")
    TArray<UUnitAbility*> GetUnitAbilities(bool bIncludeHidden = false);

    UFUNCTION(BlueprintCallable, Category = "Actor")
    void FaceUnit(AUnit* OtherUnit);

public:
    virtual void BeginPlay() override;
    virtual void EndPlay(EEndPlayReason::Type Reason) override;

protected:

    UFUNCTION(BlueprintNativeEvent)
    void OnDeath();

    UFUNCTION(BlueprintNativeEvent, Category = "Ability")
    FGameplayAbilitySpecHandle GetPreferredAttackAbility() const;

    // From the given array of available attack availabilities, select which one the unit would prefer to use.
    // The default implementation picks the most damaging ability.
    // Overrides could do something like check if a certain high-priority ability is available and use that,
    // and otherwise fall back to default.
    // Note abilities in param are shallow copies and should not be cached or mutated
    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Ability")
    const int PickPreferredAttackAbility(const TArray<FGameplayAbilitySpec>& AttackAbilities) const;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
    TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Attributes")
    TObjectPtr<ULordUnitAttributeSet> LordUnitAttributeSet;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
    EUnitTeam Team;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability")
    TArray<TSubclassOf<UUnitAbility>> DefaultAbilities;

    UPROPERTY(EditDefaultsOnly, Category = "Attributes", meta = (RequiredAssetDataTags = "RowStructure=/Script/ProjectLord.UnitBaseAttributes"))
    TObjectPtr<UDataTable> ClassAttributeDefaults;

    virtual void SetupBaseAttributes();

private:

};
