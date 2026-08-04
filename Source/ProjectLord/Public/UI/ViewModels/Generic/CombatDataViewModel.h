// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UI/ViewModels/LordViewModelBase.h"

#include "CombatDataViewModel.generated.h"

class UCombatComponent;

UCLASS(BlueprintType)
class PROJECTLORD_API UVMCombatData: public UVMLordBase
{
    GENERATED_BODY()

public:
    int GetHealth() const { return Health; }
    int GetMaxHealth() const { return MaxHealth; }
    int GetMana() const { return Mana; }
    int GetMaxMana() const { return MaxMana; }

    int GetMeleeDefense() const { return MeleeDefense; }
    int GetRangedDefense() const { return RangedDefense; }
    int GetMagicDefense() const { return MagicDefense; }

    int GetMeleeDamage() const { return MeleeDamage; }
    int GetRangedDamage() const { return RangedDamage; }
    int GetMagicDamage() const { return MagicDamage; }

    float GetSight() const { return Sight; }
    float GetAttackRange() const { return AttackRange; }

    UCombatComponent* GetTarget() const { return Target; }

    virtual void Init(UCombatComponent* Component);

    static UVMCombatData* Make(UObject* Outer, UCombatComponent* Component)
    {
        auto VM = CreateLordVM<UVMCombatData>(Outer);
        VM->Init(Component);
        return VM;
    }

protected:

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int Health;
    void SetHealth(int InHealth) { UE_MVVM_SET_PROPERTY_VALUE(Health, InHealth); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int MaxHealth;
    void SetMaxHealth(int InMaxHealth) { UE_MVVM_SET_PROPERTY_VALUE(MaxHealth, InMaxHealth); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int Mana;
    void SetMana(int InMana) { UE_MVVM_SET_PROPERTY_VALUE(Mana, InMana); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int MaxMana;
    void SetMaxMana(int InMaxMana) { UE_MVVM_SET_PROPERTY_VALUE(MaxMana, InMaxMana); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int MeleeDefense;
    void SetMeleeDefense(int InMeleeDefense) { UE_MVVM_SET_PROPERTY_VALUE(MeleeDefense, InMeleeDefense); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int RangedDefense;
    void SetRangedDefense(int InRangedDefense) { UE_MVVM_SET_PROPERTY_VALUE(RangedDefense, InRangedDefense); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int MagicDefense;
    void SetMagicDefense(int InMagicDefense) { UE_MVVM_SET_PROPERTY_VALUE(MagicDefense, InMagicDefense); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int MeleeDamage;
    void SetMeleeDamage(int InMeleeDamage) { UE_MVVM_SET_PROPERTY_VALUE(MeleeDamage, InMeleeDamage); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int RangedDamage;
    void SetRangedDamage(int InRangedDamage) { UE_MVVM_SET_PROPERTY_VALUE(RangedDamage, InRangedDamage); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    int MagicDamage;
    void SetMagicDamage(int InMagicDamage) { UE_MVVM_SET_PROPERTY_VALUE(MagicDamage, InMagicDamage); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    float Sight;
    void SetSight(float InSight) { UE_MVVM_SET_PROPERTY_VALUE(Sight, InSight); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    float AttackRange;
    void SetAttackRange(float InAttackRange) { UE_MVVM_SET_PROPERTY_VALUE(AttackRange, InAttackRange); }

    UPROPERTY(FieldNotify, BlueprintReadOnly, Getter, Category = "Combat Data")
    UCombatComponent* Target;
    void SetTarget(UCombatComponent* InTarget) { UE_MVVM_SET_PROPERTY_VALUE(Target, InTarget); }

    UFUNCTION()
    void OnTargetChange(UCombatComponent* InTarget) { SetTarget(InTarget); }

};
