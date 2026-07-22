// Copyright (c) Skyler Manzanares. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Character.h"

#include "Gameplay/Combat/CombatTypes.h"
#include "Gameplay/Units/UnitTypes.h"

#include "Unit.generated.h"

class AUnitController;

UCLASS(Blueprintable)
class PROJECTLORD_API AUnit : public ACharacter
{
    GENERATED_BODY()

public:
    AUnit();


    UFUNCTION(BlueprintPure)
    AUnitController* GetUnitController() const;

    UFUNCTION(BlueprintPure)
    float GetHealthPercent() const { return FMath::Clamp((double)Health / (double)MaxHealth, 0.0, 1.0); }

    UFUNCTION(BlueprintPure)
    bool IsDead() const { return Health <= 0; }

    UFUNCTION(BlueprintPure)
    bool IsAlive() const { return !IsDead(); }

    UFUNCTION(BlueprintPure, Category = "Combat")
    bool IsCloseEnoughToAttack(const AUnit* OtherUnit) const;

    UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category = "Combat|State")
    bool CanAttack() const;

    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetDefenseFor(EDamageType InType) const { return GetDefenses().GetDefense(InType); }
    
    // Get this unit's slash defense with all buffs and equipment factored in
    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetSlashDefense() const { return GetDefenseFor(EDamageType::Slash); }

    // Get this unit's pierce defense with all buffs and equipment factored in
    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetPierceDefense() const { return GetDefenseFor(EDamageType::Pierce); }

    // Get this unit's magic defense with all buffs and equipment factored in
    UFUNCTION(BlueprintPure, Category = "Combat")
    int GetMagicDefense() const { return GetDefenseFor(EDamageType::Magic); }

    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void DamageUnit(FAttackDamage InDamage);

    // Attack the passed in unit as this unit
    UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Combat")
    void AttackUnit(AUnit* TargetUnit);

protected:

    UFUNCTION(BlueprintNativeEvent)
    void OnDeath();

    // Return the combined Defenses of this unit. This includes all active effects, equipment, etc.
    UFUNCTION(BlueprintPure, BlueprintNativeEvent, Category = "Combat")
    FDefenses GetDefenses() const;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    double WanderRadius;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    double Sight;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Attributes")
    double AttackRange;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta=(ClampMin=1))
    int MaxHealth;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
    int Health;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats", meta=(ClampMin=1))
    int Level;

    UPROPERTY(EditDefaultsOnly, Category = "Defense")
    FDefenses BaseDefense;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team")
    EUnitTeam Team;


private:
    // Used internally to track if an attack is charging/animating
    UPROPERTY()//Category = "Combat"
    bool bIsAttacking;

    // Similarly to bIsAttacking, used internally to remember information about an attack in progress
    UPROPERTY()//Category = "Combat"
    TObjectPtr<AUnit> AttackTargetUnit;

    FTimerHandle TempAttackTimerHandle;

};
