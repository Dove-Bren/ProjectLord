// Copyright (c) Project Contributors. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h" // Needed for getter/setter macros
#include "CombatAttributeSet.generated.h"

#define DEF_REP_ATTRIBUTE(Class, Field) \
    {\
        GAMEPLAYATTRIBUTE_REPNOTIFY(Class, Field, OldValue);\
    }

#define ROUND_ATTRIB_TO_INT_CONCAT_INNER(a, b) a##b
#define ROUND_ATTRIB_TO_INT_CONCAT(a, b) ROUND_ATTRIB_TO_INT_CONCAT_INNER(a, b)
#define ROUND_ATTRIB_TO_INT(AttributeToMatch) \
    if (Attribute == ROUND_ATTRIB_TO_INT_CONCAT(Get, ROUND_ATTRIB_TO_INT_CONCAT(AttributeToMatch, Attribute))()) { NewValue = FMath::RoundToInt(NewValue); }

UCLASS()
class PROJECTLORD_API UCombatAttributeSet : public UAttributeSet
{
    GENERATED_BODY()

public:

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
    virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
    virtual void PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue) override;
    virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;

protected:

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Level, Category = "Attributes|Stats")
    FGameplayAttributeData Level = 1;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes|Stats")
    FGameplayAttributeData MaxHealth = 10;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes|Stats")
    FGameplayAttributeData Health = 10;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Attributes|Stats")
    FGameplayAttributeData MaxMana = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Attributes|Stats")
    FGameplayAttributeData Mana = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MeleeDefense, Category = "Attributes|Defense")
    FGameplayAttributeData MeleeDefense = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RangedDefense, Category = "Attributes|Defense")
    FGameplayAttributeData RangedDefense = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MagicDefense, Category = "Attributes|Defense")
    FGameplayAttributeData MagicDefense = 0;

    // Bonus melee damage, like from equipment or class abilities
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BonusMeleeDamage, Category = "Attributes|Combat Modifiers")
    FGameplayAttributeData BonusMeleeDamage = 0;

    // Bonus rannged damage, like from equipment or class abilities
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BonusRangedDamage, Category = "Attributes|Combat Modifiers")
    FGameplayAttributeData BonusRangedDamage = 0;

    // Bonus magic damage, like from equipment or class abilities
    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BonusMagicDamage, Category = "Attributes|Combat Modifiers")
    FGameplayAttributeData BonusMagicDamage = 0;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Sight, Category = "Attributes|AI Characteristics")
    FGameplayAttributeData Sight = 1000;

    UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackRange, Category = "Attributes|AI Characteristics")
    FGameplayAttributeData AttackRange = 100;

    UFUNCTION()
    virtual void OnRep_Level(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, Level)

    UFUNCTION()
    virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, MaxHealth)

    UFUNCTION()
    virtual void OnRep_Health(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, Health)

    UFUNCTION()
    virtual void OnRep_MaxMana(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, MaxMana)

    UFUNCTION()
    virtual void OnRep_Mana(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, Mana)

    UFUNCTION()
    virtual void OnRep_MeleeDefense(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, MeleeDefense)

    UFUNCTION()
    virtual void OnRep_RangedDefense(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, RangedDefense)

    UFUNCTION()
    virtual void OnRep_MagicDefense(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, MagicDefense)

    UFUNCTION()
    virtual void OnRep_BonusMeleeDamage(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, BonusMeleeDamage)

    UFUNCTION()
    virtual void OnRep_BonusRangedDamage(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, BonusRangedDamage)

    UFUNCTION()
    virtual void OnRep_BonusMagicDamage(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, BonusMagicDamage)

    UFUNCTION()
    virtual void OnRep_Sight(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, Sight)

    UFUNCTION()
    virtual void OnRep_AttackRange(const FGameplayAttributeData& OldValue) DEF_REP_ATTRIBUTE(UCombatAttributeSet, AttackRange)


public:

    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Level);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, MaxHealth);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Health);

    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, MaxMana);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Mana);

    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, MeleeDefense);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, RangedDefense);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, MagicDefense);

    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, BonusMeleeDamage);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, BonusRangedDamage);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, BonusMagicDamage);

    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, Sight);
    ATTRIBUTE_ACCESSORS_BASIC(UCombatAttributeSet, AttackRange);

};

#undef DEF_REP_ATTRIBUTE
